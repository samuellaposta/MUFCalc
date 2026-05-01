#pragma once
#include <QDialog>
#include <QPainter>
#include "WatermarkWidget.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include "../core/Types.h"

/** A virtual nuclear facility replica for NMA training */
struct VirtualFacility {
    QString id;
    QString name;
    QString type;           // e.g. "UF6 Enrichment Plant"
    QString country;
    QString material;
    QString description;
    QString icon;
    // ── Recommendation #3 (Prof. Pushkarsky / reviewer): anchor each
    //    facility on ONE primary KMP for the worked example so figures
    //    are consistent across the educational walkthrough.
    QString primaryKmp;     // e.g. "KMP-B (Product UF6 cylinders)"
    QString primaryKmpRationale;
    // ── Recommendation #2: per-facility table of measurement methods
    //    & typical 1σ — drawn from STR-368. HTML fragment for direct
    //    rendering in the Scenario tab.
    QString methodsTableHtml;
    // Pre-built MBP scenarios for this facility
    struct Scenario {
        QString id;
        QString name;
        QString description;
        QString difficulty;  // Beginner / Intermediate / Advanced
        QString learnObjective;
        MUFCalc::MufInput input;
        QString expectedOutcome; // What the student should observe
    };
    QVector<Scenario> scenarios;
};

/**
 * VirtualFacilityDialog
 *
 * 4 pre-built virtual nuclear facility replicas:
 *   1. UF6 Enrichment Plant (Russia/Rosatom type)
 *   2. MOX Fuel Fabrication Facility
 *   3. Spent Fuel Reprocessing Plant
 *   4. Research Reactor & Radioisotope Production
 *
 * Each has 3 NMA scenarios: normal, anomalous, critical.
 * Students select a facility + scenario, study the data, then
 * load it into the main calculator for full analysis.
 */
class VirtualFacilityDialog : public QDialog {
    Q_OBJECT
public:
    explicit VirtualFacilityDialog(QWidget* parent = nullptr);

signals:
    /** Emitted when user clicks "Load into Calculator" */
    void loadScenario(const MUFCalc::MufInput& input);

private slots:
    void onFacilitySelected(int row);
    void onScenarioSelected(int row);
    void onLoadClicked();
    void onRunSimulation();
    void onRunNMACalculation();    // ← NEW: live step-by-step NMA

protected:
    void paintEvent(QPaintEvent* e) override;
private:
    void buildUI();
    void buildFacilities();
    QWidget* buildFacilityPanel();
    QWidget* buildScenarioPanel();
    QWidget* buildDetailPanel();
    void populateScenarioList();
    void updateDetailView();
    void updateSimulationView();

    // Facility data
    QVector<VirtualFacility> m_facilities;
    int m_selectedFacility  = 0;
    int m_selectedScenario  = 0;

    // UI widgets
    QListWidget*    m_facilityList  = nullptr;
    QListWidget*    m_scenarioList  = nullptr;
    QStackedWidget* m_detailStack   = nullptr;
    QLabel*         m_facName       = nullptr;
    QLabel*         m_facType       = nullptr;
    QLabel*         m_facDesc       = nullptr;
    QTextBrowser*   m_scenarioBrowser = nullptr;
    QTextBrowser*   m_nmaDataBrowser  = nullptr;
    QTextBrowser*   m_nmaLiveBrowser  = nullptr;   // ← NEW
    QLabel*         m_stepLabels[5]   = {};          // ← NEW: step indicator
    QPushButton*    m_nmaRunBtn       = nullptr;     // ← NEW
    QLabel*         m_diffBadge     = nullptr;
    QLabel*         m_objectiveLabel = nullptr;
    QPushButton*    m_loadBtn       = nullptr;
    QPushButton*    m_simBtn        = nullptr;
    QLabel*         m_simStatus     = nullptr;
    QProgressBar*   m_simProgress   = nullptr;
};
