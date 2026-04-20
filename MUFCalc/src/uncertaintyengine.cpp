#include "uncertaintyengine.h"
#include <cmath>
#include <algorithm>

double UncertaintyEngine::combinedVariance(const QVector<NuclearItem>& items) {
    double v = 0;
    for (auto& it : items) v += variance(it);
    return v;
}

double UncertaintyEngine::combinedSigma(const QVector<NuclearItem>& items) {
    return std::sqrt(combinedVariance(items));
}

void UncertaintyEngine::compute(const MBPInput& in, MUFResult& r) const {
    double varBI  = variance(in.beginningInventory);
    double varIn  = combinedVariance(in.inputs);
    double varOut = combinedVariance(in.outputs);
    r.varianceMUF  = varBI + varIn + varOut;
    r.sigmaMUF     = std::sqrt(r.varianceMUF);
    r.sigmaBI      = std::sqrt(varBI);
    r.sigmaInputs  = std::sqrt(varIn);
    r.sigmaOutputs = std::sqrt(varOut);
    r.contributions = buildContributions(in, r.varianceMUF);
}

QVector<UncertaintyContribution>
UncertaintyEngine::buildContributions(const MBPInput& in, double varMUF) const {
    QVector<UncertaintyContribution> out;
    if (varMUF <= 0) return out;
    auto push = [&](const QString& lbl, double v) {
        out.append({ lbl, v, (v / varMUF) * 100.0 });
    };
    push(QString("BI: %1").arg(in.beginningInventory.label.isEmpty()
                               ? "Beginning Inventory" : in.beginningInventory.label),
         variance(in.beginningInventory));
    for (auto& it : in.inputs)
        push(QString("Input: %1").arg(it.label.isEmpty() ? "Unnamed" : it.label), variance(it));
    for (auto& it : in.outputs)
        push(QString("Output: %1").arg(it.label.isEmpty() ? "Unnamed" : it.label), variance(it));
    std::sort(out.begin(), out.end(),
              [](const UncertaintyContribution& a, const UncertaintyContribution& b){
                  return a.percentContrib > b.percentContrib; });
    return out;
}
