#pragma once
#include <QDialog>
#include <QPainter>
#include "WatermarkWidget.h"
#include <QTabWidget>
#include <QTextBrowser>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>

class StudentGuideDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentGuideDialog(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* e) override;
private:
    void buildUI();
    QWidget* buildHowToUse();             // ← NEW: visual landing walkthrough
    QWidget* buildInteractiveTutorial();
    void     buildLessons();
    void     showLesson(int idx);
    void     markComplete(int idx);
    QWidget* buildQuickStart();
    QWidget* buildTheory();
    QWidget* buildDiagnosticsGuide();
    QWidget* buildGlossary();
    QWidget* buildFAQ();
    static QTextBrowser* makeBrowser(const QString& html);

    QListWidget*    m_lessonList    = nullptr;
    QStackedWidget* m_lessonStack   = nullptr;
    QProgressBar*   m_progress      = nullptr;
    QLabel*         m_progressLbl   = nullptr;
    QPushButton*    m_btnPrev       = nullptr;
    QPushButton*    m_btnNext       = nullptr;
    int             m_currentLesson = 0;
    int             m_totalLessons  = 0;
    QVector<bool>   m_completed;

    struct Lesson { QString title; QString icon; QString html; };
    QVector<Lesson> m_lessons;
};
