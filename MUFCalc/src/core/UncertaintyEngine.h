#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: UncertaintyEngine
//  GUM-compliant 1σ propagation:
//      σ²(MUF) = σ²(BI) + Σσ²(In) + Σσ²(Out) + σ²(EI)
//  Refs: GUM JCGM 100:2008, IAEA Services Series 15, STR-368.
// ─────────────────────────────────────────────────────────────────────────

#include "Types.h"

namespace MUFCalc {

class UncertaintyEngine {
public:
    /// Populates sigma* and varianceMuf fields on the result in-place.
    void compute(const MufInput& input, MufResult& result) const;

    static double variance(const InventoryComponent& it) {
        return it.uncertainty * it.uncertainty;
    }
    static double combinedVariance(const QVector<InventoryComponent>& items);
    static double combinedSigma   (const QVector<InventoryComponent>& items);
};

}  // namespace MUFCalc
