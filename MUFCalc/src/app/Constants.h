#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: System Constants
//  IAEA Safeguards Glossary 2022 §3.14, §3.17;
//  STR-368 ITV 2010; INFCIRC/153.
// ─────────────────────────────────────────────────────────────────────────

namespace MUFCalc {

// ── IAEA Significant Quantities (kg) ────────────────────────────────────
struct SignificantQuantity {
    static constexpr double Pu      =  8.0;
    static constexpr double U233    =  8.0;
    static constexpr double HEU     = 25.0;     // ≥ 20 % U-235
    static constexpr double LEU     = 75.0;     //  < 20 % U-235
    static constexpr double NatU    = 10000.0;
    static constexpr double DeplU   = 20000.0;
    static constexpr double Th      = 20000.0;
    static constexpr double MOX     =  8.0;     // Pu-bearing
    static constexpr double Default = 75.0;
};

// ── IAEA Timeliness detection goals (days) ──────────────────────────────
struct TimelinessGoal {
    static constexpr int DirectUse   = 30;      // Pu, U-233, HEU
    static constexpr int IndirectUse = 90;      // LEU, NatU, DeplU, Th
};

// ── Severity / detection thresholds ─────────────────────────────────────
struct DetectionThresholds {
    static constexpr double AlertSigma     =  2.0;   // 2σ → WARNING
    static constexpr double CriticalSigma  =  3.0;   // 3σ → CRITICAL
    static constexpr double DominantSource = 50.0;   // % of σ²(MUF)
    static constexpr double ThroughputPct  =  5.0;   // |MUF|/throughput
    static constexpr double SqWarnFraction = 0.25;
    static constexpr double SqAlertFraction= 0.50;
    static constexpr double CusumThreshold =  3.0;   // Page's CUSUM
    static constexpr double SitmufZ95      = 1.645;  // 5 % one-tailed
    static constexpr double UncertaintyRatioMax = 10.0;
};

// ── Input validation hard limits (anti-injection) ───────────────────────
struct ValidationLimits {
    static constexpr double MAX_WEIGHT       = 1.0e8;  // 100 kt
    static constexpr double MAX_UNCERTAINTY  = 1.0e6;  //   1 kt
    static constexpr int    MAX_NAME_LEN     = 200;
    static constexpr int    MAX_ITEMS        = 100;
    static constexpr int    MAX_ID_LEN       = 50;
    static constexpr int    MAX_OPERATOR_LEN = 100;
};

}  // namespace MUFCalc
