#include "mufengine.h"
#include <QDateTime>
#include <cmath>

// Exact implementation of Flowchart (Image 2):
// LEFT:  Validate → IB = BI+ΣIn-ΣOut → MUF = IB-EI
// RIGHT: σ(MUF) → Cᵢ analysis → Safeguards Diagnostics
// FINAL: Explainable MUF Result
MUFResult MUFEngine::calculate(const MBPInput& input) {
    MUFResult r;
    r.calculatedAt = QDateTime::currentDateTime();

    // Step 1: IAEA Validation Rules / Physical Consistency Checks
    m_lastVal = m_val.validate(input);
    if (!m_lastVal.isEmpty()) {
        QStringList msgs;
        for (auto& v : m_lastVal) msgs << QString("\u2022 [%1] %2").arg(v.field, v.issue);
        r.errorMessage = "Input validation failed:\n" + msgs.join("\n");
        return r;
    }

    // Step 2R (right branch): Uncertainty Propagation σ(MUF)
    //   AND Uncertainty Contribution Analysis Cᵢ = σ²(i)/σ²(MUF)×100%
    m_unc.compute(input, r);

    // Step 3 (left): Book Inventory  IB = BI + ΣInputs − ΣOutputs
    r.sumInputs = r.sumOutputs = 0;
    for (auto& it : input.inputs)  r.sumInputs  += it.qty;
    for (auto& it : input.outputs) r.sumOutputs += it.qty;
    r.bookInventory = input.beginningInventory.qty + r.sumInputs - r.sumOutputs;

    // Step 4 (left): MUF = IB − EI
    r.muf   = r.bookInventory - input.endingInventory.qty;
    r.ratio = (r.sigmaMUF > 0) ? std::abs(r.muf) / r.sigmaMUF : 0.0;

    // Step 4R (right): Safeguards Diagnostic Rules (Threshold & Anomaly Checks)
    m_diag.run(input, r);

    r.success = true;
    return r;
}
