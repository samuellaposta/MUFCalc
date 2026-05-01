#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: StatisticalEvaluator
//  Sequential safeguards tests for protracted and abrupt diversion:
//    - SITMUF   — Standardised Independently Transformed MUF (Burr 2013)
//    - Page's CUSUM (Burr 2013, Jones 1986)
//    - CUMUF   — running ΣMUF across MBPs (OSTI-1461843)
//  Sequential state PERSISTS across MBP periods until reset().
// ─────────────────────────────────────────────────────────────────────────

#include "Types.h"

namespace MUFCalc {

class StatisticalEvaluator {
public:
    /// Updates running CUSUM/CUMUF state and writes the per-period values
    /// (sitmuf, cumuf, cusum.*) into the result.
    void evaluate(MufResult& result);

    /// Reset sequential accumulators (call on "Clear History").
    void reset();

    double cusumThreshold() const { return m_cusumThreshold; }
    void   setCusumThreshold(double t) { m_cusumThreshold = t; }

private:
    double m_cusumPos       = 0.0;   // Page S+(t) across MBPs
    double m_cusumNeg       = 0.0;   // Page S-(t) across MBPs
    double m_cumuf          = 0.0;   // running ΣMUF
    double m_cusumK         = 0.5;   // standard slack
    double m_cusumThreshold = 3.0;
};

}  // namespace MUFCalc
