#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: DiagnosticEngine
//  Generates the 12 safeguards diagnostic flags from a populated MufResult.
//  Refs: IAEA INFCIRC/153, Safeguards Glossary 2022 §3.7/§3.14/§5.38,
//        Burr et al. 2013 doi:10.1155/2013/961360, OSTI-1461843,
//        IAEA-TECDOC-1169.
// ─────────────────────────────────────────────────────────────────────────

#include "Types.h"
#include "../app/Constants.h"

namespace MUFCalc {

class DiagnosticEngine {
public:
    /// Populates result.flags based on input + already-computed stats.
    /// Assumes UncertaintyEngine, ContributionAnalyzer and
    /// StatisticalEvaluator have run first.
    void evaluate(const MufInput& input, MufResult& result) const;

    // Tunable thresholds (defaults from app/Constants.h)
    double alertSigma     = DetectionThresholds::AlertSigma;
    double criticalSigma  = DetectionThresholds::CriticalSigma;
    double dominantPct    = DetectionThresholds::DominantSource;
    double throughputPct  = DetectionThresholds::ThroughputPct;
    double sqWarn         = DetectionThresholds::SqWarnFraction;
    double sqAlert        = DetectionThresholds::SqAlertFraction;
    double sitmufZ95      = DetectionThresholds::SitmufZ95;
    double uncertaintyMax = DetectionThresholds::UncertaintyRatioMax;

private:
    void checkSigmaIntegrity     (MufResult& r) const;
    void checkMufSignificance    (MufResult& r) const;
    void checkNegativeMuf        (MufResult& r) const;
    void checkDominantSource     (MufResult& r) const;
    void checkThroughputRatio    (const MufInput& in, MufResult& r) const;
    void checkSignificantQuantity(const MufInput& in, MufResult& r) const;
    void checkCusum              (MufResult& r) const;
    void checkCumuf              (MufResult& r) const;
    void checkSitmuf             (MufResult& r) const;
    void checkUncertaintyRatio   (MufResult& r) const;
    void checkTimelinessGoal     (const MufInput& in, MufResult& r) const;

    void addFlag(MufResult& r, Severity sev,
                 const QString& code, const QString& msg,
                 const QString& action, const QString& ref = {}) const;
};

}  // namespace MUFCalc
