#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: Core Data Types
//  Refs: IAEA INFCIRC/153, Services Series No. 15 (NMA Handbook),
//        STR-368 ITV 2010, Safeguards Glossary 2022, GUM JCGM 100:2008,
//        Avenhaus 1977, Burr et al. 2013 (CUSUM/SITMUF).
// ─────────────────────────────────────────────────────────────────────────

#include <QDateTime>
#include <QString>
#include <QVector>

namespace MUFCalc {

// ── A single measured nuclear material item ─────────────────────────────
//   Sign convention (IAEA / GUM / STR-368):
//     - uncertainty is 1σ standard deviation, ALWAYS ≥ 0 (precision).
//     - bias is a SIGNED systematic offset in kg (accuracy / calibration).
//       Positive = instrument reads high; negative = reads low.
//     - relativeUncertainty is uncertainty/weight expressed as a fraction
//       (0.005 = 0.5 %). Informational only; the engine uses absolute σ.
struct InventoryComponent {
    QString name;                       // e.g. "Beginning Inventory"
    double  weight              = 0.0;  // kg, bias-corrected best estimate
    double  uncertainty         = 0.01; // 1σ absolute (kg), ≥ 0
    double  bias                = 0.0;  // signed systematic offset (kg)
    double  relativeUncertainty = 0.0;  // uncertainty / weight (fraction)
};

// ── Complete input for one Material Balance Period ──────────────────────
struct MufInput {
    QString   facilityName;
    QString   facilityId;
    QString   materialType;     // "UF6 LEU", "MOX", "HEU", ...
    QString   operatorName;
    QString   mbpId;
    QDateTime periodStart;
    QDateTime periodEnd;
    InventoryComponent           beginningInventory;
    InventoryComponent           endingInventory;
    QVector<InventoryComponent>  inputs;
    QVector<InventoryComponent>  outputs;
    int       sequenceNumber = 1;       // for CUSUM / sequential testing
};

// ── Per-term uncertainty contribution Cᵢ = σ²(i)/σ²(MUF) × 100 % ────────
struct UncertaintyContribution {
    QString name;                 // descriptive label of the term
    double  varianceContrib = 0.0;
    double  percentContrib  = 0.0;
};

// ── Diagnostic severity ─────────────────────────────────────────────────
enum class Severity { OK, INFO, WARNING, ALERT, CRITICAL };

struct DiagnosticFlag {
    Severity severity = Severity::INFO;
    QString  code;
    QString  message;
    QString  action;
    QString  reference;           // IAEA / literature citation
};

// ── Page's CUSUM running state (Burr 2013, Jones 1986) ──────────────────
struct CUSUMState {
    double positiveSum = 0.0;     // S+(t)
    double negativeSum = 0.0;     // S-(t)
    double threshold   = 3.0;
    double k           = 0.5;     // standard slack
    bool   alarmed     = false;
};

// ── Full MUF calculation result ─────────────────────────────────────────
struct MufResult {
    // IAEA equations
    double bookInventory = 0.0;   // IB = BI + ΣIn − ΣOut
    double muf           = 0.0;   // MUF = IB − EI
    double sigmaMuf      = 0.0;   // √[σ²(BI) + σ²(ΣIn) + σ²(ΣOut) + σ²(EI)]
    double varianceMuf   = 0.0;
    double sumInputs     = 0.0;
    double sumOutputs    = 0.0;
    double sigmaBI       = 0.0;
    double sigmaInputs   = 0.0;
    double sigmaOutputs  = 0.0;
    double ratio         = 0.0;   // |MUF| / σ(MUF)

    // Explainable AI: contribution analysis
    QVector<UncertaintyContribution> contributions;
    QString dominantSource;       // highest-contributing term name

    // Diversion detection diagnostics
    double     sitmuf       = 0.0;        // MUF / σ(MUF)
    double     cumuf        = 0.0;        // running ΣMUF across MBPs
    CUSUMState cusum;                     // Page's CUSUM state
    double     sqFraction   = 0.0;        // |MUF| / SQ
    bool       sqExceeded   = false;
    double     sigQuantity  = 75.0;       // SQ for material (kg)

    QVector<DiagnosticFlag> flags;
    bool      success = false;
    QString   errorMessage;
    QDateTime calculatedAt;
};

// ── Validation result (one per failed field) ────────────────────────────
struct ValidationResult {
    bool    valid = true;
    QString field;
    QString issue;
};

}  // namespace MUFCalc
