#pragma once
// ══════════════════════════════════════════════════════════════════════
//  MeasurementMethodDB  —  IAEA STR-368 / NMA Handbook Type B
//  Uncertainty Database (header-only, no .cpp needed)
//
//  Source:  IAEA STR-368 "Measurement Uncertainties in Nuclear
//           Safeguards", Table 3.1 and related sections.
//           IAEA NMA Handbook, Services Series No.15, §3.4.
//           GUM (JCGM 100:2008) Type B evaluation methodology.
//
//  Usage:
//    auto methods = MeasurementMethodDB::methodsForMaterial("UF6 (Uranium Hexafluoride)");
//    // populate a QComboBox; store key via setItemData(i, method.key)
//    // when selection changes: err = epsilon_nominal * qty
// ══════════════════════════════════════════════════════════════════════

#include <QString>
#include <QVector>

// ─────────────────────────────────────────────────────────────────────
//  Single measurement method entry
// ─────────────────────────────────────────────────────────────────────
struct MeasurementMethod {
    QString key;             ///< Unique identifier (used as QComboBox item data)
    QString displayName;     ///< Human-readable name shown in combo
    double  epsilonNominal;  ///< Nominal relative std uncertainty (fraction, NOT %)
    double  epsilonMin;      ///< Typical lower bound
    double  epsilonMax;      ///< Typical upper bound
    QString iaeaRef;         ///< Normative IAEA reference
    QString hint;            ///< Tooltip / guidance text
};

// ─────────────────────────────────────────────────────────────────────
//  Static database — one QVector per material type
// ─────────────────────────────────────────────────────────────────────
class MeasurementMethodDB {
public:

    /** Sentinel: user enters σ directly — no auto-update */
    static MeasurementMethod manualEntry() {
        return { "manual", "✏  Manual Entry (σ below)",
                 0.0, 0.0, 0.0, "", "Enter 1σ uncertainty directly in the field below" };
    }

    /**
     * Returns the ordered list of applicable measurement methods for
     * the given material type string (as used in MUFCalc's combo box).
     * The "Manual Entry" sentinel is always appended last.
     */
    static QVector<MeasurementMethod> methodsForMaterial(const QString& mat)
    {
        QString m = mat.toLower();
        QVector<MeasurementMethod> v;

        // ── UF6 ────────────────────────────────────────────────────
        if (m.contains("uf6")) {
            v = {
                { "uf6_cw",
                  "Cylinder Weighing (tare + gross)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.2.1",
                  "Gross minus tare on calibrated balance; IAEA standard UF6 accountability method" },
                { "uf6_idms",
                  "Mass Spectrometry (IDMS)",
                  0.0005, 0.0003, 0.0010,
                  "STR-368 §3.2.3",
                  "Isotope Dilution Mass Spectrometry; highest accuracy for uranium assay" },
                { "uf6_fm",
                  "Flow Meter (process liquid UF6)",
                  0.0100, 0.0050, 0.0150,
                  "STR-368 §3.2.5",
                  "Coriolis or magnetic flow meters on liquid process streams" },
                { "uf6_vol",
                  "Volumetric (calibrated cylinder)",
                  0.0030, 0.0015, 0.0050,
                  "STR-368 §3.2.2",
                  "Volume + density measurement for liquid heel or vapour" },
            };
        }
        // ── UO2 ────────────────────────────────────────────────────
        else if (m.contains("uo2")) {
            v = {
                { "uo2_grav",
                  "Gravimetric (laboratory balance)",
                  0.0010, 0.0005, 0.0020,
                  "STR-368 §3.3.1",
                  "High-precision balance; primary method for solid UO2 pellets and powder" },
                { "uo2_tit",
                  "Chemical Titration (Davies-Gray)",
                  0.0015, 0.0010, 0.0025,
                  "STR-368 §3.3.2 / ASTM C1267",
                  "Potentiometric titration; certified reference material required" },
                { "uo2_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "Non-destructive assay; suitable for verification, not primary accountability" },
            };
        }
        // ── LEU ────────────────────────────────────────────────────
        else if (m.contains("leu") || (m.contains("low enriched"))) {
            v = {
                { "leu_grav",
                  "Gravimetric (laboratory balance)",
                  0.0010, 0.0005, 0.0020,
                  "STR-368 §3.3.1",
                  "Preferred for solid LEU; calibrated 4-decimal balance required" },
                { "leu_tit",
                  "Chemical Titration (Davies-Gray / K2Cr2O7)",
                  0.0015, 0.0010, 0.0025,
                  "STR-368 §3.3.2",
                  "Dissolved UO2 or UF6; CRM spike required for IDMS confirmation" },
                { "leu_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "Useful for LEU rods / assemblies in-situ; larger uncertainty than DA" },
                { "leu_nc",
                  "NDA — Neutron Coincidence Counting",
                  0.0100, 0.0050, 0.0200,
                  "STR-368 §4.3",
                  "Applicable to LEU powder or scrap; needs matrix correction" },
            };
        }
        // ── HEU ────────────────────────────────────────────────────
        else if (m.contains("heu") || m.contains("highly enriched")) {
            v = {
                { "heu_grav",
                  "Gravimetric (laboratory balance)",
                  0.0010, 0.0005, 0.0015,
                  "STR-368 §3.3.1",
                  "Primary method for HEU metal or oxide in controlled environment" },
                { "heu_tit",
                  "Chemical Titration (Davies-Gray)",
                  0.0015, 0.0010, 0.0020,
                  "STR-368 §3.3.2",
                  "Dissolved HEU; IAEA verification requires two independent DA methods" },
                { "heu_idms",
                  "Mass Spectrometry (IDMS)",
                  0.0005, 0.0003, 0.0010,
                  "STR-368 §3.2.3",
                  "Highest accuracy for HEU assay; required for strategic-level quantities" },
                { "heu_nc",
                  "NDA — Neutron Coincidence Counting",
                  0.0100, 0.0050, 0.0200,
                  "STR-368 §4.3",
                  "Used at KMPs for in-situ verification of HEU items" },
                { "heu_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "Enrichment verification; combine with mass measurement for full accountability" },
            };
        }
        // ── Natural Uranium ─────────────────────────────────────────
        else if (m.contains("natural uranium")) {
            v = {
                { "natu_grav",
                  "Gravimetric (tank / drum weighing)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.3.1",
                  "Calibrated platform balance; standard for natural UO3 / UF4 drums" },
                { "natu_vol",
                  "Volumetric (calibrated tank, liquid)",
                  0.0030, 0.0020, 0.0050,
                  "STR-368 §3.2.2",
                  "Level gauge + density meter for liquid process streams" },
                { "natu_tit",
                  "Chemical Titration (Davies-Gray)",
                  0.0015, 0.0010, 0.0025,
                  "STR-368 §3.3.2",
                  "Standard DA method for dissolved uranium ores and solutions" },
                { "natu_xrf",
                  "XRF / Fluorimetry",
                  0.0050, 0.0030, 0.0100,
                  "STR-368 §4.4",
                  "Suitable for dilute uranium solutions and environmental sampling" },
            };
        }
        // ── Depleted Uranium ────────────────────────────────────────
        else if (m.contains("depleted")) {
            v = {
                { "du_grav",
                  "Gravimetric (cylinder / drum weighing)",
                  0.0020, 0.0010, 0.0040,
                  "STR-368 §3.3.1",
                  "Platform balance; DU tails cylinders weighed gross and tare" },
                { "du_vol",
                  "Volumetric (calibrated tank)",
                  0.0040, 0.0020, 0.0060,
                  "STR-368 §3.2.2",
                  "For liquid DUF6 or UNH process streams; larger ε due to density variation" },
                { "du_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0250, 0.0150, 0.0350,
                  "STR-368 §4.2",
                  "Higher ε for DU due to low U-235 signal; useful for confirmation only" },
                { "du_nc",
                  "NDA — Neutron Coincidence Counting",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.3",
                  "Applicable for DU scrap and waste drums" },
            };
        }
        // ── Plutonium ───────────────────────────────────────────────
        else if (m.contains("plutonium") || (m.contains("pu") && !m.contains("mox"))) {
            v = {
                { "pu_grav",
                  "Glovebox Gravimetric (controlled atmosphere)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.4.1",
                  "Primary DA method; requires inert atmosphere glovebox for oxide/metal" },
                { "pu_tit",
                  "Chemical Titration (K2Cr2O7 / cerimetry)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.4.2",
                  "Dissolution + controlled potential coulometry; ASTM C1104 / C759" },
                { "pu_idms",
                  "Mass Spectrometry (IDMS)",
                  0.0005, 0.0003, 0.0010,
                  "STR-368 §3.2.3",
                  "Highest accuracy; required for Pu in direct-use categories" },
                { "pu_nc",
                  "NDA — Neutron Coincidence Counting (AWCC/PSMC)",
                  0.0100, 0.0050, 0.0200,
                  "STR-368 §4.3",
                  "Multiplicity counting preferred for heterogeneous items" },
                { "pu_gamma",
                  "NDA — Gamma Spectrometry (HRGS)",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "Pu isotopic ratio by HRGS; combine with mass for full assay" },
            };
        }
        // ── MOX ────────────────────────────────────────────────────
        else if (m.contains("mox")) {
            v = {
                { "mox_grav",
                  "Gravimetric (controlled atmosphere balance)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.4.1",
                  "Primary method for MOX powder and pellets; Pu fraction from DA" },
                { "mox_nc",
                  "NDA — Neutron Coincidence Counting",
                  0.0100, 0.0050, 0.0200,
                  "STR-368 §4.3",
                  "AWCC or PSMC on MOX items; matrix effect correction required" },
                { "mox_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0250, 0.0150, 0.0350,
                  "STR-368 §4.2",
                  "Pu/U ratio verification; larger ε due to overlapping gamma lines" },
                { "mox_tit",
                  "Chemical Titration (dissolved MOX)",
                  0.0020, 0.0015, 0.0030,
                  "STR-368 §3.4.2",
                  "Separate U and Pu assay after dissolution; ASTM C697" },
            };
        }
        // ── U-233 ───────────────────────────────────────────────────
        else if (m.contains("u-233") || m.contains("u233")) {
            v = {
                { "u233_grav",
                  "Gravimetric (controlled atmosphere balance)",
                  0.0010, 0.0005, 0.0020,
                  "STR-368 §3.4.1",
                  "Primary DA method; treat as direct-use material — strict controls" },
                { "u233_tit",
                  "Chemical Titration",
                  0.0015, 0.0010, 0.0025,
                  "STR-368 §3.3.2",
                  "Davies-Gray or cerimetric titration after dissolution" },
                { "u233_idms",
                  "Mass Spectrometry (IDMS)",
                  0.0005, 0.0003, 0.0010,
                  "STR-368 §3.2.3",
                  "Required for verification at IAEA significant quantity boundary (8 kg)" },
                { "u233_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "208Tl daughter used as proxy; corrections needed for ingrowth" },
            };
        }
        // ── Thorium ─────────────────────────────────────────────────
        else if (m.contains("thorium") || m.contains("th")) {
            v = {
                { "th_grav",
                  "Gravimetric (laboratory balance)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.3.1",
                  "Standard method for ThO2 powder and metal; drum or batch weighing" },
                { "th_tit",
                  "Chemical Titration (complexometric)",
                  0.0020, 0.0015, 0.0030,
                  "STR-368 §3.5",
                  "EDTA complexometric titration; ASTM C1210 reference method" },
                { "th_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0300, 0.0200, 0.0500,
                  "STR-368 §4.2",
                  "228Ac or 208Tl daughters; larger ε due to equilibrium delays" },
                { "th_xrf",
                  "XRF Analysis",
                  0.0050, 0.0030, 0.0100,
                  "STR-368 §4.4",
                  "Suitable for dissolved thorium nitrate solutions" },
            };
        }
        // ── Fallback: generic methods ───────────────────────────────
        else {
            v = {
                { "gen_grav",
                  "Gravimetric (laboratory balance)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.3.1",
                  "Generic gravimetric; adjust ε based on balance specification" },
                { "gen_tit",
                  "Chemical Titration (DA)",
                  0.0020, 0.0010, 0.0030,
                  "STR-368 §3.3.2",
                  "Generic destructive analysis; CRM required" },
                { "gen_gamma",
                  "NDA — Gamma Spectrometry",
                  0.0200, 0.0100, 0.0300,
                  "STR-368 §4.2",
                  "Generic NDA gamma; method-specific correction factors apply" },
            };
        }

        // Always offer Manual as final fallback
        v.append(manualEntry());
        return v;
    }

    /**
     * Look up a method by its key within the methods for a given material.
     * Returns manualEntry() if not found.
     */
    static MeasurementMethod findByKey(const QString& key, const QString& materialType = QString())
    {
        QVector<MeasurementMethod> pool = materialType.isEmpty()
            ? allKnownMethods()
            : methodsForMaterial(materialType);
        for (auto& m : pool)
            if (m.key == key) return m;
        return manualEntry();
    }

private:
    /** Flat list of every defined method — used only by findByKey() */
    static QVector<MeasurementMethod> allKnownMethods()
    {
        // Iterate all material types and collect unique entries
        static const QStringList matTypes = {
            "UF6 (Uranium Hexafluoride)", "UO2 (Uranium Dioxide)",
            "LEU (Low Enriched Uranium, <20%)", "HEU (Highly Enriched Uranium, ≥20%)",
            "Natural Uranium", "Depleted Uranium (DU)", "Plutonium (Pu)",
            "MOX (Mixed Oxide Fuel)", "U-233", "Thorium (Th)"
        };
        QVector<MeasurementMethod> all;
        QStringList seen;
        for (auto& t : matTypes) {
            for (auto& m : methodsForMaterial(t)) {
                if (!seen.contains(m.key)) { seen.append(m.key); all.append(m); }
            }
        }
        return all;
    }
};
