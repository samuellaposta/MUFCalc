#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: MufCalculator
//  Top-level orchestrator. Pipeline:
//    1. Validator               — input + physical-consistency checks
//    2. UncertaintyEngine       — σ(MUF) propagation
//    3. Book Inventory & MUF    — IB = BI + ΣIn − ΣOut, MUF = IB − EI
//    4. ContributionAnalyzer    — Cᵢ for explainable AI
//    5. StatisticalEvaluator    — SITMUF, CUSUM, CUMUF
//    6. DiagnosticEngine        — 12 safeguards flags
// ─────────────────────────────────────────────────────────────────────────

#include "Types.h"
#include "UncertaintyEngine.h"
#include "ContributionAnalyzer.h"
#include "StatisticalEvaluator.h"
#include "DiagnosticEngine.h"
#include "../utils/Validator.h"

namespace MUFCalc {

class MufCalculator {
public:
    MufResult calculate(const MufInput& input);

    const QVector<ValidationResult>& lastValidation() const { return m_lastVal; }
    Validator&             validator()   { return m_validator; }
    UncertaintyEngine&     uncertainty() { return m_unc; }
    ContributionAnalyzer&  contributions(){ return m_contrib; }
    StatisticalEvaluator&  statistics()  { return m_stats; }
    DiagnosticEngine&      diagnostics() { return m_diag; }

    /// Reset sequential CUSUM/CUMUF state (call on "Clear History").
    void resetSequentialState() { m_stats.reset(); }

private:
    Validator             m_validator;
    UncertaintyEngine     m_unc;
    ContributionAnalyzer  m_contrib;
    StatisticalEvaluator  m_stats;
    DiagnosticEngine      m_diag;
    QVector<ValidationResult> m_lastVal;
};

}  // namespace MUFCalc
