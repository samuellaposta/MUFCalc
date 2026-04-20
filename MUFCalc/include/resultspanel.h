#pragma once
#include <QDialog>
#include <QPainter>
#include "watermarkwidget.h"
#include <QTabWidget>
#include <QLabel>
#include <QTableWidget>
#include <QTextBrowser>
#include "datatypes.h"

/**
 * ResultsPanel — 4-tab results display:
 *   Tab 1: Summary  (MUF banner + stat cards)
 *   Tab 2: Equations (step-by-step IAEA substitution)
 *   Tab 3: Uncertainty (animated contribution bars)
 *   Tab 4: Diagnostics (colour-coded flags)
 */
class ResultsPanel : public QDialog {
    Q_OBJECT
public:
    explicit ResultsPanel(const MBPInput& input,
                          const MUFResult& result,
                          QWidget* parent = nullptr);

private slots:
    void onExportHTML();
    void onExportText();

protected:
    void paintEvent(QPaintEvent* e) override;
private:
    void buildUI();
    QWidget* buildSummaryTab();
    QWidget* buildEquationsTab();
    QWidget* buildUncertaintyTab();
    QWidget* buildDiagnosticsTab();

    QString ratioColorCSS() const;
    QString statusLabel()   const;

    const MBPInput&  m_input;
    const MUFResult& m_result;
    QTabWidget*      m_tabs;
};
