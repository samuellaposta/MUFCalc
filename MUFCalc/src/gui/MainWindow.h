#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QStackedWidget>
#include "../core/MufCalculator.h"
#include "../reports/ReportGenerator.h"
#include "../database/HistoryRepository.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private slots:
    void onNewCalculation();
    void onLoadExample();
    void onExportReport();
    void onClearHistory();
    void onAbout();
    void onSendFeedback();
    void onIAEARef();
    void onDiversionGuide();
    void onShowFlowchart();
    void onShowEquations();
    void onUserGuide();
    void onVirtualFacility();
    void onCalculationDone(const MUFCalc::MufInput& input, const MUFCalc::MufResult& result);
    void onHistoryClicked(QListWidgetItem* item);
private:
    void buildUI();
    void buildMenuBar();
    QWidget* buildWelcomePage();
    QWidget* buildDashboard();
    void refreshDashboard(const MUFCalc::MufInput& input, const MUFCalc::MufResult& result);
    void refreshHistory();
    void setStatus(const QString& msg, const QString& color = "#15803d");

    MUFCalc::MufCalculator       m_engine;
    MUFCalc::ReportGenerator m_reporter;
    MUFCalc::HistoryRepository  m_history;

    // Dashboard widgets
    QStackedWidget* m_stack;
    QLabel*         m_dashMUF;
    QLabel*         m_dashSigma;
    QLabel*         m_dashIB;
    QLabel*         m_dashSQ;
    QLabel*         m_dashStatus;
    QLabel*         m_dashFacility;
    QTextEdit*      m_dashDiag;
    QListWidget*    m_historyList;
    QLabel*         m_statusLbl;

    MUFCalc::MufInput  m_lastInput;
    MUFCalc::MufResult m_lastResult;
};
