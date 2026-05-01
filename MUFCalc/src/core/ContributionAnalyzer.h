#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: ContributionAnalyzer  (Explainable AI layer)
//  Cᵢ = σ²(i) / σ²(MUF) × 100 %
//  Identifies which measurement dominates total MUF uncertainty.
// ─────────────────────────────────────────────────────────────────────────

#include "Types.h"

namespace MUFCalc {

class ContributionAnalyzer {
public:
    /// Builds a sorted (descending percent) list of per-term contributions
    /// and stores it on r.contributions, also populates r.dominantSource.
    void analyze(const MufInput& input, MufResult& result) const;
};

}  // namespace MUFCalc
