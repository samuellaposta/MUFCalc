#include "ResultsPanel.h"
#include "SummaryTab.h"
#include "EquationsTab.h"
#include "UncertaintyTab.h"
#include "DiagnosticsTab.h"
#include "WatermarkWidget.h"
#include "../app/Styles.h"
#include "../reports/ReportGenerator.h"
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MUFCalc;

ResultsPanel::ResultsPanel(const MufInput& input,
                            const MufResult& result,
                            QWidget* parent)
    : QDialog(parent), m_input(input), m_result(result)
{
    setWindowTitle(QStringLiteral("MUF Results — Explainable Safeguards Output"));
    setMinimumSize(820, 660);
    setStyleSheet(Styles::appStyle() + Styles::lineEditStyle());
    buildUI();
}

void ResultsPanel::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);

    // Header strip
    auto* hdr = new QFrame();
    hdr->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                       "stop:0 #1a3a5c,stop:1 #2d5680);border-bottom:2px solid #d1d5db;");
    auto* hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(20, 14, 20, 14);
    auto* htitle = new QLabel(QStringLiteral("\u269b  Explainable MUF Results"));
    htitle->setStyleSheet("color:#fff;font-size:16px;font-weight:900;");
    auto* hsub = new QLabel(m_input.facilityName + "  |  " + m_input.mbpId + "  |  " +
                            m_result.calculatedAt.toString("yyyy-MM-dd HH:mm"));
    hsub->setStyleSheet("color:#cbd5e1;font-size:13px;");
    hl->addWidget(htitle); hl->addStretch(); hl->addWidget(hsub);
    root->addWidget(hdr);

    // Tabs (each tab is a standalone QWidget subclass)
    m_tabs = new QTabWidget();
    m_tabs->setStyleSheet(Styles::tabStyle());
    m_tabs->addTab(new SummaryTab     (m_input, m_result), QStringLiteral("\U0001F4CA  Summary"));
    m_tabs->addTab(new EquationsTab   (m_input, m_result), QStringLiteral("\U0001F522  Equations"));
    m_tabs->addTab(new UncertaintyTab (m_input, m_result), QStringLiteral("\U0001F4C9  Uncertainty"));
    m_tabs->addTab(new DiagnosticsTab (m_input, m_result), QStringLiteral("\U0001F50D  Diagnostics"));
    root->addWidget(m_tabs, 1);

    // Footer
    auto* footer = new QFrame();
    footer->setStyleSheet("background:white;border-top:1px solid #e5e7eb;");
    auto* fl = new QHBoxLayout(footer);
    fl->setContentsMargins(16, 10, 16, 10);

    auto* btnHTML = new QPushButton(QStringLiteral("\U0001F4BE  Export HTML Report"));
    btnHTML->setStyleSheet(Styles::secondaryButtonStyle());
    auto* btnTxt = new QPushButton(QStringLiteral("\U0001F4C4  Export Text Report"));
    btnTxt->setStyleSheet(Styles::secondaryButtonStyle() +
                          "QPushButton{color:#166534;border-color:#16a34a;}");
    auto* btnClose = new QPushButton(QStringLiteral("Close"));
    btnClose->setStyleSheet("QPushButton{background:#f4f5f7;color:#374151;"
                            "border:1px solid #d1d5db;border-radius:7px;padding:8px 18px;}"
                            "QPushButton:hover{background:#e5e7eb;}");

    fl->addWidget(btnHTML); fl->addWidget(btnTxt); fl->addStretch(); fl->addWidget(btnClose);
    root->addWidget(footer);

    connect(btnHTML,  &QPushButton::clicked, this, &ResultsPanel::onExportHTML);
    connect(btnTxt,   &QPushButton::clicked, this, &ResultsPanel::onExportText);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

void ResultsPanel::onExportHTML() {
    const QString suggested = QString("MUF_%1_%2.html")
        .arg(m_input.mbpId.isEmpty() ? "report" : m_input.mbpId)
        .arg(m_result.calculatedAt.toString("yyyyMMdd_HHmmss"));
    const QString path = QFileDialog::getSaveFileName(this,
        QStringLiteral("Export HTML Report"), suggested,
        QStringLiteral("HTML Files (*.html)"));
    if (path.isEmpty()) return;

    ReportGenerator gen;
    if (gen.saveHtml(path, m_input, m_result))
        QMessageBox::information(this, QStringLiteral("Export Complete"),
                                 QStringLiteral("HTML report saved to:\n") + path);
    else
        QMessageBox::warning(this, QStringLiteral("Export Failed"),
                             QStringLiteral("Could not write file."));
}

void ResultsPanel::onExportText() {
    const QString suggested = QString("MUF_%1_%2.txt")
        .arg(m_input.mbpId.isEmpty() ? "report" : m_input.mbpId)
        .arg(m_result.calculatedAt.toString("yyyyMMdd_HHmmss"));
    const QString path = QFileDialog::getSaveFileName(this,
        QStringLiteral("Export Text Report"), suggested,
        QStringLiteral("Text Files (*.txt)"));
    if (path.isEmpty()) return;

    ReportGenerator gen;
    if (gen.savePlainText(path, m_input, m_result))
        QMessageBox::information(this, QStringLiteral("Export Complete"),
                                 QStringLiteral("Text report saved to:\n") + path);
    else
        QMessageBox::warning(this, QStringLiteral("Export Failed"),
                             QStringLiteral("Could not write file."));
}

void ResultsPanel::paintEvent(QPaintEvent* e) {
    QDialog::paintEvent(e);
    QPainter p(this);
    IAEAWatermark::paint(p, rect());
}
