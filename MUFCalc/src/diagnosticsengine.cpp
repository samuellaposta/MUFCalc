#include "diagnosticsengine.h"
#include <cmath>

// ══════════════════════════════════════════════════════════════════════
//  DiagnosticsEngine — Flowchart Image 2 right-branch implementation
//  + comprehensive diversion detection from:
//    Burr et al. (2013) doi:10.1155/2013/961360
//    OSTI-1461843 (Safeguards Performance Analysis, ORNL 2018)
//    IAEA Safeguards Glossary 2022
//    INMM 2021 (State-level diversion detection)
// ══════════════════════════════════════════════════════════════════════

void DiagnosticsEngine::run(const MBPInput& in, MUFResult& r) const {
    r.flags.clear();
    // ── Flowchart: Safeguards Diagnostic Rules ─────────────────────────
    checkSigmaIntegrity(r);
    checkMUFSignificance(r);
    checkNegativeMUF(r);
    checkDominantSource(r);
    checkThroughputRatio(in, r);
    // ── Literature: Diversion detection ───────────────────────────────
    checkSignificantQuantity(in, r);
    checkCUSUM(r);
    checkCUMUF(r);
    checkSITMUF(r);
    checkUncertaintyRatio(r);
    checkTimelinessGoal(in, r);

    // Emit OK only if nothing more severe was flagged
    bool hasSevere = false;
    for (auto& f : r.flags)
        if (f.severity != Severity::INFO) { hasSevere = true; break; }
    if (r.flags.isEmpty() || !hasSevere)
        addFlag(r, Severity::OK, "MUF-NORMAL",
            QString("|MUF| = %1 kg is within 1\u03c3(MUF) = %2 kg. "
                    "Normal range — no diversion indicated.")
                .arg(std::abs(r.muf),0,'f',4).arg(r.sigmaMUF,0,'f',4),
            "No action required. Routine nuclear material accounting verified.",
            "IAEA INFCIRC/153; Safeguards Glossary 2022 \u00a75.38");
}

void DiagnosticsEngine::checkSigmaIntegrity(MUFResult& r) const {
    if (r.sigmaMUF <= 0)
        addFlag(r, Severity::CRITICAL, "ZERO-SIGMA",
            "\u03c3(MUF) = 0. All uncertainties are zero — physically impossible.",
            "Enter realistic 1\u03c3 absolute uncertainties for all items. "
            "Consult IAEA measurement error guidelines.",
            "IAEA Safeguards Glossary 2022 \u00a710.11");
}

void DiagnosticsEngine::checkMUFSignificance(MUFResult& r) const {
    if (r.sigmaMUF <= 0) return;
    double ra = r.ratio;
    if (ra > criticalThreshold)
        addFlag(r, Severity::CRITICAL,
            QString("MUF-EXCEED-%1\u03c3").arg((int)criticalThreshold),
            QString("|MUF| = %1 kg exceeds %2\u03c3 = %3 kg (ratio %4\u03c3). "
                    "SIGNIFICANT ANOMALY — possible diversion of nuclear material.")
                .arg(std::abs(r.muf),0,'f',4).arg(criticalThreshold,0,'f',0)
                .arg(criticalThreshold*r.sigmaMUF,0,'f',4).arg(ra,0,'f',2),
            "IMMEDIATELY notify safeguards authority. "
            "Initiate physical inventory verification. Preserve all records.",
            "IAEA INFCIRC/153 Art.29; Safeguards Glossary 2022 \u00a73.7");
    else if (ra > alertThreshold)
        addFlag(r, Severity::WARNING,
            QString("MUF-EXCEED-%1\u03c3").arg((int)alertThreshold),
            QString("|MUF| = %1 kg exceeds %2\u03c3 = %3 kg (ratio %4\u03c3). Elevated concern.")
                .arg(std::abs(r.muf),0,'f',4).arg(alertThreshold,0,'f',0)
                .arg(alertThreshold*r.sigmaMUF,0,'f',4).arg(ra,0,'f',2),
            "Review all transfer records, measurement logs, and operator declarations "
            "for this MBP. Schedule early inspection.",
            "IAEA Safeguards Glossary 2022 \u00a75.38; INFCIRC/153 Art.76");
    else if (ra > 1.0)
        addFlag(r, Severity::INFO, "MUF-EXCEED-1\u03c3",
            QString("|MUF| = %1 kg is in the 1\u03c3\u20132\u03c3 range (ratio %2\u03c3). "
                    "Within statistical expectation (\u03b1\u22640.32 false alarm rate).")
                .arg(std::abs(r.muf),0,'f',4).arg(ra,0,'f',2),
            "Monitor next MBP closely. No immediate action required.",
            "Burr et al. (2013) doi:10.1155/2013/961360");
}

void DiagnosticsEngine::checkNegativeMUF(MUFResult& r) const {
    if (r.muf < -r.sigmaMUF * 0.5)
        addFlag(r, Severity::ALERT, "NEGATIVE-MUF",
            QString("MUF = %1 kg is negative — apparent material gain of %2 kg. "
                    "May indicate unrecorded inputs or measurement bias.")
                .arg(r.muf,0,'f',4).arg(std::abs(r.muf),0,'f',4),
            "Verify EI measurement and calibration. Check for unrecorded inputs. "
            "Review all shipper-receiver differences.",
            "IAEA Nuclear Material Accounting Handbook (2008) Sec.4.3");
}

void DiagnosticsEngine::checkDominantSource(MUFResult& r) const {
    if (!r.contributions.isEmpty() && r.contributions[0].percentContrib >= dominantThreshold)
        addFlag(r, Severity::WARNING, "DOMINANT-UNCERTAINTY-SOURCE",
            QString("'%1' contributes %2% to \u03c3\u00b2(MUF) — dominant uncertainty source. "
                    "Limits diversion detection sensitivity.")
                .arg(r.contributions[0].label)
                .arg(r.contributions[0].percentContrib,0,'f',1),
            QString("Prioritise re-calibration of '%1'. "
                    "Apply NDA/DA methods to reduce this contribution.")
                .arg(r.contributions[0].label),
            "OSTI-1461843; IAEA-TECDOC-1169");
}

void DiagnosticsEngine::checkThroughputRatio(const MBPInput& in, MUFResult& r) const {
    double tp = 0;
    for (auto& it : in.inputs)  tp += it.qty;
    for (auto& it : in.outputs) tp += it.qty;
    if (tp > 0) {
        double pct = (std::abs(r.muf) / tp) * 100.0;
        if (pct > throughputLimit)
            addFlag(r, Severity::ALERT, "HIGH-MUF-TO-THROUGHPUT",
                QString("|MUF| = %1% of total throughput (%2 kg). "
                        "Above the %3% threshold.")
                    .arg(pct,0,'f',2).arg(tp,0,'f',2).arg(throughputLimit,0,'f',0),
                "Audit all transfer measurements. Verify KMP data.",
                "IAEA INFCIRC/153 Art.29; ORNL/TM-2012/531");
    }
}

void DiagnosticsEngine::checkSignificantQuantity(const MBPInput& in, MUFResult& r) const {
    double sq = sqForMaterial(in.materialType);
    r.sigQuantity = sq;
    double am = std::abs(r.muf);
    r.sqFraction  = am / sq;
    r.sqExceeded  = (r.sqFraction >= 1.0);
    if (r.sqExceeded)
        addFlag(r, Severity::CRITICAL, "SQ-EXCEEDED",
            QString("|MUF| = %1 kg meets or exceeds 1 Significant Quantity (%2 kg) "
                    "for %3. Weapons-relevant quantity unaccounted for.")
                .arg(am,0,'f',4).arg(sq,0,'f',1).arg(in.materialType),
            "IMMEDIATELY report to State authority and IAEA. "
            "This constitutes a potential diversion — Art.25 INFCIRC/153.",
            "IAEA Safeguards Glossary 2022 \u00a73.14; INFCIRC/153 Art.25");
    else if (r.sqFraction >= sqAlertFraction)
        addFlag(r, Severity::ALERT, "SQ-50PCT",
            QString("|MUF| = %1% of one SQ (%2 kg for %3). Elevated diversion concern.")
                .arg(r.sqFraction*100,0,'f',1).arg(sq,0,'f',1).arg(in.materialType),
            "Notify safeguards authority. Verify before next MBP.",
            "IAEA Safeguards Glossary 2022 \u00a73.14");
    else if (r.sqFraction >= sqWarnFraction)
        addFlag(r, Severity::WARNING, "SQ-25PCT",
            QString("|MUF| = %1% of one SQ (%2 kg). Elevated monitoring required.")
                .arg(r.sqFraction*100,0,'f',1).arg(sq,0,'f',1),
            "Increase measurement frequency. Document all material flows carefully.",
            "IAEA Safeguards Glossary 2022 \u00a73.14");
}

// Page's CUSUM — Burr et al. (2013); Jones (1986) IAEA-SM-293/23
// S+(t) = max(0, S+(t-1) + SITMUF(t) - k)
// S-(t) = max(0, S-(t-1) - SITMUF(t) - k)
// State persists across MBP periods; reset via resetSequentialState()
void DiagnosticsEngine::checkCUSUM(MUFResult& r) const {
    if (r.sigmaMUF <= 0) return;
    r.sitmuf = r.muf / r.sigmaMUF;
    const double k = 0.5;
    m_cusumPos = std::max(0.0, m_cusumPos + r.sitmuf - k);
    m_cusumNeg = std::max(0.0, m_cusumNeg - r.sitmuf - k);
    // Store current running values into result for display/export
    r.cusum.k            = k;
    r.cusum.threshold    = cusumThreshold;
    r.cusum.positiveSum  = m_cusumPos;
    r.cusum.negativeSum  = m_cusumNeg;
    r.cusum.alarmed      = (m_cusumPos > cusumThreshold || m_cusumNeg > cusumThreshold);
    if (m_cusumPos > cusumThreshold) {
        addFlag(r, Severity::CRITICAL, "CUSUM-ALARM-LOSS",
            QString("Page's CUSUM S+ = %1 exceeds threshold %2. "
                    "Systematic material LOSS detected across balance periods.")
                .arg(m_cusumPos,0,'f',3).arg(cusumThreshold,0,'f',1),
            "Protracted diversion may be occurring. Initiate special inspection. "
            "Notify IAEA inspectors. Review all previous MBPs.",
            "Burr et al. (2013) doi:10.1155/2013/961360; Jones (1986) IAEA-SM-293/23");
    } else if (m_cusumNeg > cusumThreshold) {
        addFlag(r, Severity::ALERT, "CUSUM-ALARM-GAIN",
            QString("Page's CUSUM S\u2212 = %1 exceeds threshold %2. "
                    "Systematic material GAIN \u2014 possible undeclared inputs.")
                .arg(m_cusumNeg,0,'f',3).arg(cusumThreshold,0,'f',1),
            "Check for undeclared nuclear material inputs. "
            "Verify all receipts and shipper-receiver differences.",
            "Burr et al. (2013) doi:10.1155/2013/961360");
    }
}

// Cumulative MUF — OSTI-1461843
// State persists across MBP periods; reset via resetSequentialState()
void DiagnosticsEngine::checkCUMUF(MUFResult& r) const {
    m_cumuf += r.muf;
    r.cumuf = m_cumuf;
    if (r.sigmaMUF > 0 && std::abs(m_cumuf) > 2.0 * r.sigmaMUF)
        addFlag(r, Severity::WARNING, "CUMUF-ELEVATED",
            QString("CUMUF = %1 kg exceeds 2\u03c3 = %2 kg. "
                    "Sustained accumulation may indicate protracted diversion.")
                .arg(m_cumuf,0,'f',4).arg(2.0*r.sigmaMUF,0,'f',4),
            "Review all previous MBPs for systematic bias. "
            "Apply sequential CUSUM test over multiple periods.",
            "OSTI-1461843 (ORNL Safeguards Performance Analysis 2018)");
}

// SITMUF — Standardised Independently Transformed MUF (Burr 2013)
void DiagnosticsEngine::checkSITMUF(MUFResult& r) const {
    if (r.sigmaMUF <= 0) return;
    // |SITMUF| > 1.645: 5% one-tailed, α = 0.05
    if (std::abs(r.sitmuf) > 1.645)
        addFlag(r, Severity::INFO, "SITMUF-ELEVATED",
            QString("SITMUF = %1. |SITMUF| > 1.645 exceeds the 5%% significance level (\u03b1=0.05).")
                .arg(r.sitmuf,0,'f',3),
            "Statistically significant at 5% level. "
            "False alarm unlikely (\u03b1 = 0.05 per balance period).",
            "Burr et al. (2013); IAEA Safeguards Glossary 2022 \u00a710.27");
}

// Uncertainty quality vs detectability
void DiagnosticsEngine::checkUncertaintyRatio(MUFResult& r) const {
    if (r.sigmaMUF <= 0 || r.muf == 0) return;
    if (r.sigmaMUF / std::abs(r.muf) > 10.0)
        addFlag(r, Severity::WARNING, "POOR-UNCERTAINTY-RATIO",
            QString("\u03c3(MUF) = %1 kg is >10× |MUF| = %2 kg. "
                    "Measurement uncertainties too large for effective diversion detection.")
                .arg(r.sigmaMUF,0,'f',4).arg(std::abs(r.muf),0,'f',4),
            "Improve measurement systems to reduce \u03c3(MUF). "
            "Target \u03c3(MUF) < 1 SQ per IAEA guidance.",
            "IAEA-TECDOC-1169; Miller (1990) NCI Report");
}

// IAEA timeliness detection goal check
void DiagnosticsEngine::checkTimelinessGoal(const MBPInput& in, MUFResult& r) const {
    int days = in.periodStart.daysTo(in.periodEnd);
    bool direct = in.materialType.contains("Pu",   Qt::CaseInsensitive) ||
                  in.materialType.contains("HEU",  Qt::CaseInsensitive) ||
                  in.materialType.contains("U-233",Qt::CaseInsensitive);
    int goal = direct ? TimelinessGoal::DirectUse : TimelinessGoal::IndirectUse;
    if (days > goal)
        addFlag(r, Severity::INFO, "TIMELINESS-GOAL",
            QString("MBP length (%1 days) exceeds IAEA timeliness goal of %2 days for %3.")
                .arg(days).arg(goal).arg(in.materialType),
            "Consider more frequent balance closures to meet IAEA timely detection requirements.",
            "IAEA Safeguards Glossary 2022 \u00a73.17; INFCIRC/153 Art.76");
}


// Reset sequential CUSUM/CUMUF accumulators — call on "Clear History"
void DiagnosticsEngine::resetSequentialState() {
    m_cusumPos = 0.0;
    m_cusumNeg = 0.0;
    m_cumuf    = 0.0;
}
double DiagnosticsEngine::sqForMaterial(const QString& mt) const {
    QString m = mt.toLower();
    if (m.contains("pu") || m.contains("mox"))          return SignificantQuantity::Pu;
    if (m.contains("u-233"))                             return SignificantQuantity::U233;
    if (m.contains("heu") || m.contains("high enrich")) return SignificantQuantity::HEU;
    if (m.contains("depleted") || m.contains(" du"))    return SignificantQuantity::DeplU;
    if (m.contains("natural") || m.contains("natu"))    return SignificantQuantity::NatU;
    if (m.contains("th") || m.contains("thorium"))      return SignificantQuantity::Th;
    return SignificantQuantity::LEU;  // default: most common
}

void DiagnosticsEngine::addFlag(MUFResult& r, Severity sev,
                                const QString& code, const QString& msg,
                                const QString& action, const QString& ref) const {
    r.flags.append({sev, code, msg, action, ref});
}
