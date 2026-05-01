#include "MufCalculator.h"
#include <QDateTime>
#include <cmath>

namespace MUFCalc {

MufResult MufCalculator::calculate(const MufInput& input) {
    MufResult r;
    r.calculatedAt = QDateTime::currentDateTime();

    // 1. Validation
    m_lastVal = m_validator.validate(input);
    if (!m_lastVal.isEmpty()) {
        QStringList msgs;
        for (const auto& v : m_lastVal)
            msgs << QString("\u2022 [%1] %2").arg(v.field, v.issue);
        r.errorMessage = "Input validation failed:\n" + msgs.join("\n");
        return r;
    }

    // 2. σ(MUF) propagation
    m_unc.compute(input, r);

    // 3. Book Inventory and MUF
    r.sumInputs = r.sumOutputs = 0.0;
    for (const auto& it : input.inputs)  r.sumInputs  += it.weight;
    for (const auto& it : input.outputs) r.sumOutputs += it.weight;
    r.bookInventory = input.beginningInventory.weight + r.sumInputs - r.sumOutputs;
    r.muf   = r.bookInventory - input.endingInventory.weight;
    r.ratio = (r.sigmaMuf > 0.0) ? std::abs(r.muf) / r.sigmaMuf : 0.0;

    // 4. Explainable AI
    m_contrib.analyze(input, r);

    // 5. Sequential statistics (running state)
    m_stats.evaluate(r);

    // 6. Safeguards diagnostic flags
    m_diag.evaluate(input, r);

    r.success = true;
    return r;
}

}  // namespace MUFCalc
