#pragma once
#include <QString>
#include <QVector>
#include <QDateTime>

// ══════════════════════════════════════════════════════════════════════
//  IAEA Nuclear Material Accounting — Core Data Structures
//  Ref: IAEA Safeguards Glossary 2022; INFCIRC/153; Burr et al. 2013
// ══════════════════════════════════════════════════════════════════════

/** IAEA Significant Quantities (kg) — Safeguards Glossary 2022 §3.14 */
struct SignificantQuantity {
    static constexpr double Pu     =  8.0;
    static constexpr double U233   =  8.0;
    static constexpr double HEU    = 25.0;   // ≥20% U-235
    static constexpr double LEU    = 75.0;   // <20% U-235
    static constexpr double NatU   = 10000.0;
    static constexpr double DeplU  = 20000.0;
    static constexpr double Th     = 20000.0;
    static constexpr double MOX    =  8.0;   // treat as Pu-bearing
    static constexpr double Default = 75.0;
};

/** IAEA Timeliness detection goals (days) — Safeguards Glossary 2022 §3.17 */
struct TimelinessGoal {
    static constexpr int DirectUse   = 30;   // Pu, U-233, HEU
    static constexpr int IndirectUse = 90;   // LEU, NatU, DeplU, Th
};

/** Input validation hard limits (security / anti-injection) */
struct ValidationLimits {
    static constexpr double MAX_QTY   = 1.0e8;  // 100,000 tonnes
    static constexpr double MAX_ERROR = 1.0e6;  // 1,000 tonnes
    static constexpr int    MAX_LABEL = 200;     // chars
    static constexpr int    MAX_ITEMS = 100;     // rows per table
    static constexpr int    MAX_FACID = 50;
    static constexpr int    MAX_OPID  = 100;
};

/** A single measured nuclear material item */
struct NuclearItem {
    QString label;
    double  qty   = 0.0;
    double  error = 0.01;   // 1-sigma absolute (kg)
};

/** Complete input for one Material Balance Period */
struct MBPInput {
    QString     facilityName;
    QString     facilityId;
    QString     materialType;
    QString     operatorName;
    QString     mbpId;
    QDateTime   periodStart;
    QDateTime   periodEnd;
    NuclearItem beginningInventory;
    NuclearItem endingInventory;
    QVector<NuclearItem> inputs;
    QVector<NuclearItem> outputs;
    int         sequenceNumber = 1;   // for CUSUM tracking
};

/** Per-term uncertainty contribution Cᵢ = σ²(i)/σ²(MUF)×100% */
struct UncertaintyContribution {
    QString label;
    double  varianceContrib = 0.0;
    double  percentContrib  = 0.0;
};

/** Diagnostic severity */
enum class Severity { OK, INFO, WARNING, ALERT, CRITICAL };

/** Safeguards diagnostic flag */
struct DiagnosticFlag {
    Severity severity;
    QString  code;
    QString  message;
    QString  action;
    QString  reference;   // IAEA / literature citation
};

/** Page's CUSUM state — Burr et al. (2013) */
struct CUSUMState {
    double positiveSum = 0.0;   // S+(t)
    double negativeSum = 0.0;   // S-(t)
    double threshold   = 3.0;
    double k           = 0.5;   // standard slack
    bool   alarmed     = false;
};

/** Full MUF calculation results */
struct MUFResult {
    // Image 3 — IAEA equations
    double bookInventory  = 0;  // IB = BI + ΣIn − ΣOut
    double muf            = 0;  // MUF = IB − EI
    double sigmaMUF       = 0;  // σ(MUF) = √[σ²(BI)+σ²(ΣIn)+σ²(ΣOut)]
    double varianceMUF    = 0;
    double sumInputs      = 0;
    double sumOutputs     = 0;
    double sigmaBI        = 0;
    double sigmaInputs    = 0;
    double sigmaOutputs   = 0;
    double ratio          = 0;  // |MUF|/σ(MUF)

    // Image 3 — Uniqueness Layer
    QVector<UncertaintyContribution> contributions;  // Cᵢ

    // Diversion detection (from literature)
    double   sitmuf      = 0;   // MUF/σ(MUF) — Burr 2013
    double   cumuf       = 0;   // cumulative MUF — OSTI-1461843
    CUSUMState cusum;            // Page's CUSUM
    double   sqFraction  = 0;   // |MUF| / SQ
    bool     sqExceeded  = false;
    double   sigQuantity = SignificantQuantity::Default;

    QVector<DiagnosticFlag> flags;
    bool      success = false;
    QString   errorMessage;
    QDateTime calculatedAt;
};

/** Field validation result */
struct ValidationResult {
    bool    valid = true;
    QString field;
    QString issue;
};
