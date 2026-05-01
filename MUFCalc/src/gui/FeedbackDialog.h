#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: FeedbackDialog
//
//  Submits feedback directly to the MUFCalc GitHub Issues tracker via the
//  GitHub REST API. Ported from v1 (samuellaposta/MUFCalc) — same UI, same
//  flow, same submission path. The PAT is provided as a build-time
//  constant in FeedbackDialog.cpp (see GITHUB_TOKEN). Replace the
//  placeholder string with your real token before building. Do not
//  commit the real token to a public branch.
// ─────────────────────────────────────────────────────────────────────────

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QEvent>

class FeedbackDialog : public QDialog {
    Q_OBJECT
public:
    explicit FeedbackDialog(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onSubmit();
    void onStarClicked(int star);

private:
    void buildUI();
    void postToGitHub(const QString& title, const QString& body);
    void updateStars(int rating);

    QComboBox*     m_category    = nullptr;
    QTextEdit*     m_description = nullptr;
    QLineEdit*     m_nameEdit    = nullptr;
    QLineEdit*     m_emailEdit   = nullptr;
    QPushButton*   m_submitBtn   = nullptr;
    QLabel*        m_statusLabel = nullptr;
    QList<QLabel*> m_stars;
    int            m_rating = 0;

    static const QString GITHUB_TOKEN;
    static const QString GITHUB_REPO;
};
