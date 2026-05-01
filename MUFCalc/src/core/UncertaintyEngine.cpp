#include "UncertaintyEngine.h"
#include <cmath>

namespace MUFCalc {

double UncertaintyEngine::combinedVariance(const QVector<InventoryComponent>& items) {
    double v = 0.0;
    for (const auto& it : items) v += variance(it);
    return v;
}

double UncertaintyEngine::combinedSigma(const QVector<InventoryComponent>& items) {
    return std::sqrt(combinedVariance(items));
}

void UncertaintyEngine::compute(const MufInput& in, MufResult& r) const {
    const double varBI  = variance(in.beginningInventory);
    const double varIn  = combinedVariance(in.inputs);
    const double varOut = combinedVariance(in.outputs);
    const double varEI  = variance(in.endingInventory);
    r.varianceMuf  = varBI + varIn + varOut + varEI;
    r.sigmaMuf     = std::sqrt(r.varianceMuf);
    r.sigmaBI      = std::sqrt(varBI);
    r.sigmaInputs  = std::sqrt(varIn);
    r.sigmaOutputs = std::sqrt(varOut);
}

}  // namespace MUFCalc
