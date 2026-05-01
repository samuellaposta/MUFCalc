#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: ResultsPanel  (v2.0 thin shell)
//  Hosts the four standalone tab widgets:
//    SummaryTab, EquationsTab, UncertaintyTab, DiagnosticsTab.
//  Provides export buttons (HTML / plain text) and dialog chrome.
// ─────────────────────────────────────────────────────────────────────────

#include <QDialog>
#include <QTabWidget>
#include "WatermarkWidget.h"
#include "../core/Types.h"

class ResultsPanel : public QDialog {
    Q_OBJECT
public:
    explicit ResultsPanel(const MUFCalc::MufInput& input,
                          const MUFCalc::MufResult& result,
                          QWidget* parent = nullptr);

private slots:
    void onExportHTML();
    void onExportText();

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    void buildUI();
    const MUFCalc::MufInput&  m_input;
    const MUFCalc::MufResult& m_result;
    QTabWidget*               m_tabs = nullptr;
};
