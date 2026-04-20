#pragma once
#include "datatypes.h"
#include "validationengine.h"
#include "uncertaintyengine.h"
#include "diagnosticsengine.h"

/**
 * MUFEngine — orchestrates the full calculation pipeline:
 *   1. Validate  (IAEA physical consistency checks)
 *   2. Book Inventory:  IB = BI + ΣInputs − ΣOutputs
 *   3. MUF:             MUF = IB − EI
 *   4. Uncertainty:     σ(MUF) = √[σ²(BI)+σ²(In)+σ²(Out)]
 *   5. Explainability:  Cᵢ = [σ²(i)/σ²(MUF)] × 100%
 *   6. Diagnostics:     threshold & anomaly checks
 */
class MUFEngine {
public:
    MUFResult calculate(const MBPInput& input);
    const QVector<ValidationResult>& lastValidation() const { return m_lastVal; }
    ValidationEngine&  validator()   { return m_val; }
    DiagnosticsEngine& diagnostics() { return m_diag; }
private:
    ValidationEngine  m_val;
    UncertaintyEngine m_unc;
    DiagnosticsEngine m_diag;
    QVector<ValidationResult> m_lastVal;
};
