#include "DiagnosticEngine.h"
#include "SignificantQuantity.h"
#include <cmath>

namespace MUFCalc {

void DiagnosticEngine::evaluate(const MufInput& in, MufResult& r) const {
    r.flags.clear();
    // Threshold / anomaly checks
    checkSigmaIntegrity(r);
    checkMufSignificance(r);
    checkNegativeMuf(r);
    checkDominantSource(r);
    checkThroughputRatio(in, r);
    // Diversion-detection checks
    checkSignificantQuantity(in, r);
    checkCusum(r);
    checkCumuf(r);
    checkSitmuf(r);
    checkUncertaintyRatio(r);
    checkTimelinessGoal(in, r);

    // Emit MUF-NORMAL only if nothing more severe than INFO was raised
    bool hasSevere = false;
    for (const auto& f : r.flags)
        if (f.severity != Severity::INFO) { hasSevere = true; break; }
    if (r.flags.isEmpty() || !hasSevere)
        addFlag(r, Severity::OK, QStringLiteral("MUF-NORMAL"),
            QString("|MUF| = %1 kg is within 1\u03c3(MUF) = %2 kg. "
                    "Normal range — no diversion indicated.")
                .arg(std::abs(r.muf), 0, 'f', 4)
                .arg(r.sigmaMuf,    0, 'f', 4),
            QStringLiteral("No action required. Routine NMA verified."),
            QStringLiteral("IAEA INFCIRC/153; Safeguards Glossary 2022 \u00a75.38"));
}

void DiagnosticEngine::checkSigmaIntegrity(MufResult& r) const {
    if (r.sigmaMuf <= 0.0)
        addFlag(r, Severity::CRITICAL, QStringLiteral("ZERO-SIGMA"),
            QStringLiteral("\u03c3(MUF) = 0. All uncertainties are zero — physically impossible."),
            QStringLiteral("Enter realistic 1\u03c3 absolute uncertainties for all items."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a710.11"));
}

void DiagnosticEngine::checkMufSignificance(MufResult& r) const {
    if (r.sigmaMuf <= 0.0) return;
    const double ra = r.ratio;
    if (ra > criticalSigma) {
        addFlag(r, Severity::CRITICAL,
            QString("MUF-EXCEED-%1\u03c3").arg((int)criticalSigma),
            QString("|MUF| = %1 kg exceeds %2\u03c3 = %3 kg (ratio %4\u03c3). "
                    "SIGNIFICANT ANOMALY — possible diversion.")
                .arg(std::abs(r.muf), 0, 'f', 4)
                .arg(criticalSigma,   0, 'f', 0)
                .arg(criticalSigma * r.sigmaMuf, 0, 'f', 4)
                .arg(ra, 0, 'f', 2),
            QStringLiteral("IMMEDIATELY notify safeguards authority. "
                           "Initiate physical inventory verification."),
            QStringLiteral("IAEA INFCIRC/153 Art.29; Safeguards Glossary 2022 \u00a73.7"));
    } else if (ra > alertSigma) {
        addFlag(r, Severity::WARNING,
            QString("MUF-EXCEED-%1\u03c3").arg((int)alertSigma),
            QString("|MUF| = %1 kg exceeds %2\u03c3 = %3 kg (ratio %4\u03c3). Elevated concern.")
                .arg(std::abs(r.muf), 0, 'f', 4)
                .arg(alertSigma,      0, 'f', 0)
                .arg(alertSigma * r.sigmaMuf, 0, 'f', 4)
                .arg(ra, 0, 'f', 2),
            QStringLiteral("Review transfer records, measurement logs, operator declarations. "
                           "Schedule early inspection."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a75.38; INFCIRC/153 Art.76"));
    } else if (ra > 1.0) {
        addFlag(r, Severity::INFO, QStringLiteral("MUF-EXCEED-1\u03c3"),
            QString("|MUF| = %1 kg in 1\u03c3\u20132\u03c3 range (ratio %2\u03c3). "
                    "Within statistical expectation (\u03b1\u22640.32 false alarm rate).")
                .arg(std::abs(r.muf), 0, 'f', 4).arg(ra, 0, 'f', 2),
            QStringLiteral("Monitor next MBP closely. No immediate action required."),
            QStringLiteral("Burr et al. (2013) doi:10.1155/2013/961360"));
    }
}

void DiagnosticEngine::checkNegativeMuf(MufResult& r) const {
    if (r.muf < -r.sigmaMuf * 0.5)
        addFlag(r, Severity::ALERT, QStringLiteral("NEGATIVE-MUF"),
            QString("MUF = %1 kg is negative — apparent material gain of %2 kg. "
                    "May indicate unrecorded inputs or measurement bias.")
                .arg(r.muf, 0, 'f', 4).arg(std::abs(r.muf), 0, 'f', 4),
            QStringLiteral("Verify EI measurement and calibration. "
                           "Check for unrecorded inputs and shipper-receiver differences."),
            QStringLiteral("IAEA NMA Handbook (2008) Sec.4.3"));
}

void DiagnosticEngine::checkDominantSource(MufResult& r) const {
    if (!r.contributions.isEmpty() && r.contributions.first().percentContrib >= dominantPct)
        addFlag(r, Severity::WARNING, QStringLiteral("DOMINANT-UNCERTAINTY-SOURCE"),
            QString("'%1' contributes %2 %% to \u03c3\u00b2(MUF) — dominant uncertainty source. "
                    "Limits diversion-detection sensitivity.")
                .arg(r.contributions.first().name)
                .arg(r.contributions.first().percentContrib, 0, 'f', 1),
            QString("Prioritise re-calibration of '%1'. Apply NDA/DA methods to reduce its contribution.")
                .arg(r.contributions.first().name),
            QStringLiteral("OSTI-1461843; IAEA-TECDOC-1169"));
}

void DiagnosticEngine::checkThroughputRatio(const MufInput& in, MufResult& r) const {
    double tp = 0.0;
    for (const auto& it : in.inputs)  tp += it.weight;
    for (const auto& it : in.outputs) tp += it.weight;
    if (tp > 0.0) {
        const double pct = (std::abs(r.muf) / tp) * 100.0;
        if (pct > throughputPct)
            addFlag(r, Severity::ALERT, QStringLiteral("HIGH-MUF-TO-THROUGHPUT"),
                QString("|MUF| = %1 %% of total throughput (%2 kg). Above %3 %% threshold.")
                    .arg(pct, 0, 'f', 2).arg(tp, 0, 'f', 2).arg(throughputPct, 0, 'f', 0),
                QStringLiteral("Audit transfer measurements. Verify KMP data."),
                QStringLiteral("IAEA INFCIRC/153 Art.29; ORNL/TM-2012/531"));
    }
}

void DiagnosticEngine::checkSignificantQuantity(const MufInput& in, MufResult& r) const {
    const double sq = SQResolver::forMaterial(in.materialType);
    r.sigQuantity = sq;
    const double am = std::abs(r.muf);
    r.sqFraction  = (sq > 0.0) ? am / sq : 0.0;
    r.sqExceeded  = (r.sqFraction >= 1.0);

    if (r.sqExceeded)
        addFlag(r, Severity::CRITICAL, QStringLiteral("SQ-EXCEEDED"),
            QString("|MUF| = %1 kg meets or exceeds 1 SQ (%2 kg) for %3.")
                .arg(am, 0, 'f', 4).arg(sq, 0, 'f', 1).arg(in.materialType),
            QStringLiteral("IMMEDIATELY report to State authority and IAEA. "
                           "Potential diversion — Art.25 INFCIRC/153."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a73.14; INFCIRC/153 Art.25"));
    else if (r.sqFraction >= sqAlert)
        addFlag(r, Severity::ALERT, QStringLiteral("SQ-50PCT"),
            QString("|MUF| = %1 %% of one SQ (%2 kg for %3). Elevated diversion concern.")
                .arg(r.sqFraction * 100.0, 0, 'f', 1).arg(sq, 0, 'f', 1).arg(in.materialType),
            QStringLiteral("Notify safeguards authority. Verify before next MBP."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a73.14"));
    else if (r.sqFraction >= sqWarn)
        addFlag(r, Severity::WARNING, QStringLiteral("SQ-25PCT"),
            QString("|MUF| = %1 %% of one SQ (%2 kg). Elevated monitoring required.")
                .arg(r.sqFraction * 100.0, 0, 'f', 1).arg(sq, 0, 'f', 1),
            QStringLiteral("Increase measurement frequency. Document all flows carefully."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a73.14"));
}

void DiagnosticEngine::checkCusum(MufResult& r) const {
    if (r.cusum.positiveSum > r.cusum.threshold)
        addFlag(r, Severity::CRITICAL, QStringLiteral("CUSUM-ALARM-LOSS"),
            QString("Page's CUSUM S+ = %1 exceeds threshold %2. "
                    "Systematic material LOSS detected across balance periods.")
                .arg(r.cusum.positiveSum, 0, 'f', 3).arg(r.cusum.threshold, 0, 'f', 1),
            QStringLiteral("Protracted diversion may be occurring. "
                           "Initiate special inspection. Notify IAEA inspectors."),
            QStringLiteral("Burr et al. (2013); Jones (1986) IAEA-SM-293/23"));
    else if (r.cusum.negativeSum > r.cusum.threshold)
        addFlag(r, Severity::ALERT, QStringLiteral("CUSUM-ALARM-GAIN"),
            QString("Page's CUSUM S\u2212 = %1 exceeds threshold %2. "
                    "Systematic material GAIN — possible undeclared inputs.")
                .arg(r.cusum.negativeSum, 0, 'f', 3).arg(r.cusum.threshold, 0, 'f', 1),
            QStringLiteral("Check for undeclared inputs. "
                           "Verify receipts and shipper-receiver differences."),
            QStringLiteral("Burr et al. (2013) doi:10.1155/2013/961360"));
}

void DiagnosticEngine::checkCumuf(MufResult& r) const {
    if (r.sigmaMuf > 0.0 && std::abs(r.cumuf) > 2.0 * r.sigmaMuf)
        addFlag(r, Severity::WARNING, QStringLiteral("CUMUF-ELEVATED"),
            QString("CUMUF = %1 kg exceeds 2\u03c3 = %2 kg. "
                    "Sustained accumulation may indicate protracted diversion.")
                .arg(r.cumuf, 0, 'f', 4).arg(2.0 * r.sigmaMuf, 0, 'f', 4),
            QStringLiteral("Review previous MBPs for systematic bias. "
                           "Apply sequential CUSUM over multiple periods."),
            QStringLiteral("OSTI-1461843 (ORNL Safeguards Performance Analysis 2018)"));
}

void DiagnosticEngine::checkSitmuf(MufResult& r) const {
    if (r.sigmaMuf <= 0.0) return;
    if (std::abs(r.sitmuf) > sitmufZ95)
        addFlag(r, Severity::INFO, QStringLiteral("SITMUF-ELEVATED"),
            QString("SITMUF = %1. |SITMUF| > %2 exceeds the 5 %% significance level (\u03b1=0.05).")
                .arg(r.sitmuf, 0, 'f', 3).arg(sitmufZ95, 0, 'f', 3),
            QStringLiteral("Statistically significant at 5 % level. "
                           "False alarm unlikely (\u03b1 = 0.05 per balance period)."),
            QStringLiteral("Burr et al. (2013); IAEA Safeguards Glossary 2022 \u00a710.27"));
}

void DiagnosticEngine::checkUncertaintyRatio(MufResult& r) const {
    if (r.sigmaMuf <= 0.0 || r.muf == 0.0) return;
    if (r.sigmaMuf / std::abs(r.muf) > uncertaintyMax)
        addFlag(r, Severity::WARNING, QStringLiteral("POOR-UNCERTAINTY-RATIO"),
            QString("\u03c3(MUF) = %1 kg is more than %2\u00d7 |MUF| = %3 kg. "
                    "Measurement uncertainties too large for effective diversion detection.")
                .arg(r.sigmaMuf, 0, 'f', 4).arg(uncertaintyMax, 0, 'f', 0)
                .arg(std::abs(r.muf), 0, 'f', 4),
            QStringLiteral("Improve measurement systems to reduce \u03c3(MUF). "
                           "Target \u03c3(MUF) < 1 SQ per IAEA guidance."),
            QStringLiteral("IAEA-TECDOC-1169; Miller (1990) NCI Report"));
}

void DiagnosticEngine::checkTimelinessGoal(const MufInput& in, MufResult& r) const {
    const int days = in.periodStart.daysTo(in.periodEnd);
    const bool direct =
        in.materialType.contains("Pu",    Qt::CaseInsensitive) ||
        in.materialType.contains("HEU",   Qt::CaseInsensitive) ||
        in.materialType.contains("U-233", Qt::CaseInsensitive);
    const int goal = direct ? TimelinessGoal::DirectUse
                            : TimelinessGoal::IndirectUse;
    if (days > goal)
        addFlag(r, Severity::INFO, QStringLiteral("TIMELINESS-GOAL"),
            QString("MBP length (%1 days) exceeds IAEA timeliness goal of %2 days for %3.")
                .arg(days).arg(goal).arg(in.materialType),
            QStringLiteral("Consider more frequent balance closures to meet "
                           "IAEA timely-detection requirements."),
            QStringLiteral("IAEA Safeguards Glossary 2022 \u00a73.17; INFCIRC/153 Art.76"));
}

void DiagnosticEngine::addFlag(MufResult& r, Severity sev,
                               const QString& code, const QString& msg,
                               const QString& action, const QString& ref) const {
    r.flags.append({ sev, code, msg, action, ref });
}

}  // namespace MUFCalc
