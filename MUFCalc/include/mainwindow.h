#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QStackedWidget>
#include "mufengine.h"
#include "reportgenerator.h"
#include "historymanager.h"

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
    void onIAEARef();
    void onDiversionGuide();
    void onShowFlowchart();
    void onShowEquations();
    void onUserGuide();
    void onVirtualFacility();
    void onCalculationDone(const MBPInput& input, const MUFResult& result);
    void onHistoryClicked(QListWidgetItem* item);
private:
    void buildUI();
    void buildMenuBar();
    QWidget* buildWelcomePage();
    QWidget* buildDashboard();
    void refreshDashboard(const MBPInput& input, const MUFResult& result);
    void refreshHistory();
    void setStatus(const QString& msg, const QString& color = "#15803d");

    MUFEngine       m_engine;
    ReportGenerator m_reporter;
    HistoryManager  m_history;

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

    MBPInput  m_lastInput;
    MUFResult m_lastResult;
};
