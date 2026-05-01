#pragma once
#include <QDialog>
#include <QPainter>
#include "WatermarkWidget.h"
#include <QStackedWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include "../core/Types.h"
#include "MeasurementMethodDb.h"
#include "NuclearTransferItemDb.h"

/**
 * DataEntryWizard – 5-step validated data entry dialog
 * Steps: Facility | BI | Inputs | Outputs | EI
 * All fields validated with visual feedback (light gray/white theme)
 */
class DataEntryWizard : public QDialog {
    Q_OBJECT
public:
    explicit DataEntryWizard(QWidget* parent = nullptr);
    void loadExample();
    void loadScenario(const MUFCalc::MufInput& inp);   ///< Pre-populate from a virtual facility scenario
    MUFCalc::MufInput collectInput() const;

signals:
    void calculateRequested(const MUFCalc::MufInput&);

private slots:
    void nextPage();
    void prevPage();
    void onCalculate();
    void updateEquationDisplays();
    void onMaterialTypeChanged();

protected:
    void paintEvent(QPaintEvent* e) override;
private:
    void buildUI();
    QWidget* buildPage1();  // Facility
    QWidget* buildPage2();  // BI
    QWidget* buildPage3();  // Inputs
    QWidget* buildPage4();  // Outputs
    QWidget* buildPage5();  // EI
    void     addTableRow(QTableWidget* t, const QString& prefix);
    bool     validateCurrentPage();
    void     updatePageIndicator();
    // ── Type B uncertainty helpers ─────────────────────────────────
    void populateMethodCombo(QComboBox* combo, const QString& materialType,
                             const QString& preserveKey = QString());
    void applyMethodEpsilon(QComboBox* methodCombo, QDoubleSpinBox* qtyBox,
                            QDoubleSpinBox* errBox, QLabel* epsilonLbl);
    void refreshTableMethods(QTableWidget* tbl);
    // ── Transfer-item description helpers ─────────────────────────
    void refreshTableItems(QTableWidget* tbl, bool isInput);

    // Navigation
    QStackedWidget* m_pageStack    = nullptr;
    QPushButton*    m_btnPrev      = nullptr;
    QPushButton*    m_btnNext      = nullptr;
    QPushButton*    m_btnCalc      = nullptr;
    QLabel*         m_pageIndicator = nullptr;
    QLabel*         m_validationBar = nullptr;
    QVector<QLabel*> m_stepDots;

    // Page 1: Facility
    QLineEdit*      m_facilityName = nullptr;
    QLineEdit*      m_facilityId   = nullptr;
    QComboBox*      m_materialType = nullptr;
    QLineEdit*      m_operatorName = nullptr;
    QLineEdit*      m_mbpId        = nullptr;
    QDateEdit*      m_periodStart  = nullptr;
    QDateEdit*      m_periodEnd    = nullptr;

    // Page 2: BI
    QComboBox*      m_biLabel     = nullptr;   ///< Physical inventory description (editable combo)
    QDoubleSpinBox* m_biQty       = nullptr;
    QDoubleSpinBox* m_biErr       = nullptr;
    QLabel*         m_biEquation  = nullptr;
    QComboBox*      m_biMethod    = nullptr;   ///< Type B method selector (BI)
    QLabel*         m_biEpsLbl    = nullptr;   ///< Shows computed ε for BI

    // Page 3/4: Tables (4 cols: Label | Qty | Method | 1σ)
    QTableWidget*   m_inputsTable  = nullptr;
    QTableWidget*   m_outputsTable = nullptr;
    QLabel*         m_inEquation   = nullptr;
    QLabel*         m_outEquation  = nullptr;

    // Page 5: EI
    QComboBox*      m_eiLabel     = nullptr;   ///< Physical inventory description (editable combo)
    QDoubleSpinBox* m_eiQty       = nullptr;
    QDoubleSpinBox* m_eiErr       = nullptr;
    QLabel*         m_eiEquation  = nullptr;
    QComboBox*      m_eiMethod    = nullptr;   ///< Type B method selector (EI)
    QLabel*         m_eiEpsLbl    = nullptr;   ///< Shows computed ε for EI
};
