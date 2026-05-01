#include "StatisticalEvaluator.h"
#include <algorithm>
#include <cmath>

namespace MUFCalc {

void StatisticalEvaluator::evaluate(MufResult& r) {
    // SITMUF (per-period)
    r.sitmuf = (r.sigmaMuf > 0.0) ? r.muf / r.sigmaMuf : 0.0;

    // Page's CUSUM (running)
    if (r.sigmaMuf > 0.0) {
        m_cusumPos = std::max(0.0, m_cusumPos + r.sitmuf - m_cusumK);
        m_cusumNeg = std::max(0.0, m_cusumNeg - r.sitmuf - m_cusumK);
    }
    r.cusum.k           = m_cusumK;
    r.cusum.threshold   = m_cusumThreshold;
    r.cusum.positiveSum = m_cusumPos;
    r.cusum.negativeSum = m_cusumNeg;
    r.cusum.alarmed     = (m_cusumPos > m_cusumThreshold) ||
                          (m_cusumNeg > m_cusumThreshold);

    // CUMUF — OSTI-1461843
    m_cumuf += r.muf;
    r.cumuf  = m_cumuf;
}

void StatisticalEvaluator::reset() {
    m_cusumPos = 0.0;
    m_cusumNeg = 0.0;
    m_cumuf    = 0.0;
}

}  // namespace MUFCalc
