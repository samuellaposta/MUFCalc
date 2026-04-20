#pragma once
#include "datatypes.h"

// Implements ALL checks shown in flowchart Image 2 (right branch)
// PLUS diversion detection from published literature:
//   Burr et al. 2013 (CUSUM/SITMUF), OSTI-1461843, IAEA Glossary 2022
class DiagnosticsEngine {
public:
    void run(const MBPInput& in, MUFResult& r) const;
    double alertThreshold    = 2.0;
    double criticalThreshold = 3.0;
    double dominantThreshold = 50.0;
    double throughputLimit   = 5.0;
    double sqWarnFraction    = 0.25;
    double sqAlertFraction   = 0.50;
    double cusumThreshold    = 3.0;
void resetSequentialState();   ///< Call on "Clear History" — resets CUSUM & CUMUF accumulators
private:
    // Sequential state — persists across MBP periods (Burr et al. 2013)
    mutable double m_cusumPos = 0.0;   // S+(t) across periods
    mutable double m_cusumNeg = 0.0;   // S-(t) across periods
    mutable double m_cumuf    = 0.0;   // cumulative MUF across periods
    // Flowchart Image 2: "Safeguards Diagnostic Rules / Threshold & Anomaly Checks"
    void checkSigmaIntegrity      (MUFResult& r) const;
    void checkMUFSignificance     (MUFResult& r) const;
    void checkNegativeMUF         (MUFResult& r) const;
    void checkDominantSource      (MUFResult& r) const;
    void checkThroughputRatio     (const MBPInput& in, MUFResult& r) const;
    // Diversion detection (literature)
    void checkSignificantQuantity (const MBPInput& in, MUFResult& r) const;
    void checkCUSUM               (MUFResult& r) const;
    void checkCUMUF               (MUFResult& r) const;
    void checkSITMUF              (MUFResult& r) const;
    void checkUncertaintyRatio    (MUFResult& r) const;
    void checkTimelinessGoal      (const MBPInput& in, MUFResult& r) const;
    void addFlag(MUFResult& r, Severity sev,
                 const QString& code, const QString& msg,
                 const QString& action, const QString& ref = {}) const;
    double sqForMaterial(const QString& mt) const;
};
