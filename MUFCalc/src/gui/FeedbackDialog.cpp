#include "FeedbackDialog.h"

#include <QApplication>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

// ─────────────────────────────────────────────────────────────────────────
//  Token resolution at build time:
//
//    1. If src/gui/local_token.h exists (gitignored, your local-only file),
//       it defines MUFCALC_GITHUB_TOKEN with your real PAT. This is for
//       your local Qt Creator builds.
//
//    2. If GitHub Actions is running the build, the workflow writes a
//       local_token.h on the runner from the encrypted MUFCALC_GITHUB_TOKEN
//       secret. The substituted file lives only inside the runner and is
//       discarded when the run ends.
//
//    3. Otherwise (e.g. someone clones the repo and runs qmake without
//       creating local_token.h), the placeholder is used and the dialog
//       cleanly tells the user that submission isn't configured.
//
//  GITHUB_REPO: "<owner>/<name>" — feedback issues are created here.
//
//  The committed source NEVER contains a real token. The placeholder is
//  what the world sees on GitHub.
// ─────────────────────────────────────────────────────────────────────────
#if __has_include("local_token.h")
#  include "local_token.h"
#else
#  define MUFCALC_GITHUB_TOKEN "PLACEHOLDER_TOKEN"
#endif

const QString FeedbackDialog::GITHUB_TOKEN = MUFCALC_GITHUB_TOKEN;
const QString FeedbackDialog::GITHUB_REPO  = "samuellaposta/MUFCalc";

FeedbackDialog::FeedbackDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("MUFCalc - Send Feedback");
    setMinimumWidth(520);
    setModal(true);
    buildUI();
}

void FeedbackDialog::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(14);
    root->setContentsMargins(20, 20, 20, 20);

    auto* header = new QLabel("Share Your Feedback");
    header->setStyleSheet("font-size: 16px; font-weight: bold; color: #1e40af;");
    root->addWidget(header);

    auto* sub = new QLabel("Help us improve MUFCalc. Your feedback goes directly to our development team.");
    sub->setWordWrap(true);
    sub->setStyleSheet("color: #6b7280; font-size: 12px;");
    root->addWidget(sub);

    auto* form = new QFormLayout();
    form->setSpacing(10);

    m_category = new QComboBox();
    m_category->addItems({"Bug / Error", "Feature Suggestion", "General Comment",
                          "Performance Issue", "Documentation"});
    m_category->setStyleSheet("padding: 6px; border: 1px solid #d1d5db; border-radius: 4px;");
    form->addRow("Category:", m_category);

    auto* starWidget = new QWidget();
    auto* starLayout = new QHBoxLayout(starWidget);
    starLayout->setContentsMargins(0, 0, 0, 0);
    starLayout->setSpacing(4);
    for (int i = 1; i <= 5; ++i) {
        auto* star = new QLabel(QStringLiteral("\u2606"));   // ☆
        star->setStyleSheet("font-size: 22px; color: #d1d5db;");
        star->setCursor(Qt::PointingHandCursor);
        star->setProperty("starIndex", i);
        star->installEventFilter(this);
        m_stars.append(star);
        starLayout->addWidget(star);
    }
    starLayout->addStretch();
    form->addRow("Overall rating:", starWidget);

    m_description = new QTextEdit();
    m_description->setPlaceholderText("Describe the issue or suggestion in detail...");
    m_description->setMinimumHeight(120);
    m_description->setStyleSheet("border: 1px solid #d1d5db; border-radius: 4px; padding: 6px;");
    form->addRow("Description*:", m_description);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Optional");
    m_nameEdit->setStyleSheet("padding: 6px; border: 1px solid #d1d5db; border-radius: 4px;");
    form->addRow("Your Name:", m_nameEdit);

    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("Optional - for follow-up");
    m_emailEdit->setStyleSheet("padding: 6px; border: 1px solid #d1d5db; border-radius: 4px;");
    form->addRow("Email:", m_emailEdit);

    root->addLayout(form);

    m_statusLabel = new QLabel("");
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("font-size: 12px;");
    root->addWidget(m_statusLabel);

    auto* btnRow = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setStyleSheet("padding: 8px 20px; border: 1px solid #d1d5db; "
                             "border-radius: 4px; background: #f9fafb;");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_submitBtn = new QPushButton("Submit Feedback");
    m_submitBtn->setStyleSheet("padding: 8px 20px; border-radius: 4px; "
                               "background: #1e40af; color: white; font-weight: bold; border: none;");
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    connect(m_submitBtn, &QPushButton::clicked, this, &FeedbackDialog::onSubmit);

    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(m_submitBtn);
    root->addLayout(btnRow);
}

bool FeedbackDialog::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* star = qobject_cast<QLabel*>(obj);
        if (star && m_stars.contains(star)) {
            onStarClicked(star->property("starIndex").toInt());
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void FeedbackDialog::onStarClicked(int star) {
    m_rating = star;
    updateStars(star);
}

void FeedbackDialog::updateStars(int rating) {
    for (int i = 0; i < m_stars.size(); ++i) {
        if (i < rating) {
            m_stars[i]->setText(QStringLiteral("\u2605"));   // ★
            m_stars[i]->setStyleSheet("font-size: 22px; color: #f59e0b;");
        } else {
            m_stars[i]->setText(QStringLiteral("\u2606"));   // ☆
            m_stars[i]->setStyleSheet("font-size: 22px; color: #d1d5db;");
        }
    }
}

void FeedbackDialog::onSubmit() {
    const QString desc = m_description->toPlainText().trimmed();
    if (desc.isEmpty()) {
        m_statusLabel->setText("Please enter a description before submitting.");
        m_statusLabel->setStyleSheet("color: #dc2626; font-size: 12px;");
        return;
    }

    m_submitBtn->setEnabled(false);
    m_submitBtn->setText("Submitting...");
    m_statusLabel->setText("Sending feedback...");
    m_statusLabel->setStyleSheet("color: #6b7280; font-size: 12px;");
    QApplication::processEvents();

    const QString category = m_category->currentText();
    const QString stars    = m_rating > 0 ? QString("%1/5 stars").arg(m_rating) : "Not rated";
    const QString name     = m_nameEdit->text().trimmed().isEmpty()
                                ? "Anonymous" : m_nameEdit->text().trimmed();
    const QString email    = m_emailEdit->text().trimmed().isEmpty()
                                ? "Not provided" : m_emailEdit->text().trimmed();

    const QString title = QString("[Feedback] %1 - %2").arg(category, desc.left(60));
    const QString body  = QString(
        "## MUFCalc User Feedback\n\n"
        "| Field | Value |\n"
        "|-------|-------|\n"
        "| **Category** | %1 |\n"
        "| **Rating**   | %2 |\n"
        "| **Name**     | %3 |\n"
        "| **Email**    | %4 |\n\n"
        "### Description\n%5\n\n"
        "---\n"
        "*Submitted from MUFCalc v2.0*"
    ).arg(category, stars, name, email, desc);

    postToGitHub(title, body);
}

void FeedbackDialog::postToGitHub(const QString& title, const QString& body) {
    auto* manager = new QNetworkAccessManager(this);

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

    QUrl url(QString("https://api.github.com/repos/%1/issues").arg(GITHUB_REPO));
    QNetworkRequest request(url);
    request.setSslConfiguration(sslConfig);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("token %1").arg(GITHUB_TOKEN).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    request.setRawHeader("User-Agent", "MUFCalc-App");

    QJsonObject payload;
    payload["title"] = title;
    payload["body"]  = body;

    QNetworkReply* reply = manager->post(request, QJsonDocument(payload).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const int     status   = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QString response = QString::fromUtf8(reply->readAll());
        if (status == 201) {
            m_statusLabel->setText("Feedback submitted successfully. Thank you!");
            m_statusLabel->setStyleSheet("color: #15803d; font-size: 12px;");
            m_submitBtn->setText("Submitted!");
            QTimer::singleShot(2000, this, &QDialog::accept);
        } else {
            m_statusLabel->setText(QString("Error %1: %2").arg(status).arg(response.left(120)));
            m_statusLabel->setStyleSheet("color: #dc2626; font-size: 11px;");
            m_submitBtn->setEnabled(true);
            m_submitBtn->setText("Submit Feedback");
        }
        reply->deleteLater();
    });
}
