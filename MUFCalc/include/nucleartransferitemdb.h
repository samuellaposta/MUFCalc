#pragma once
// ══════════════════════════════════════════════════════════════════════
//  NuclearTransferItemDB  —  Predefined nuclear material transfer
//  descriptions for Input (receiving) and Output (distributing) tables.
//
//  Source basis:
//    IAEA INFCIRC/153 (Corrected) — paragraphs 30–32 (MBR line items)
//    IAEA NMA Handbook, Services Series No.15 (2008), §2.3 & §3.2
//    IAEA Safeguards Glossary 2022 — definitions of KMP, MBR, MUF
//    ASTM C996 / C698 — standard UF6 / UO2 transfer nomenclature
//
//  Usage:
//    auto items = NuclearTransferItemDB::inputItems("UF6 (Uranium Hexafluoride)");
//    // populate an editable QComboBox; user may also type custom text.
// ══════════════════════════════════════════════════════════════════════

#include <QString>
#include <QVector>
#include <QComboBox>
#include <QStandardItemModel>
#include <QColor>
#include <QFont>

// ─────────────────────────────────────────────────────────────────────
//  A single transfer item entry
// ─────────────────────────────────────────────────────────────────────
struct TransferItem {
    QString text;        ///< Displayed label and final text used in MBR
    QString tooltip;     ///< Guidance / IAEA reference
    bool    separator;   ///< If true, render as a visual group separator
};

// Convenience: separator factory
static inline TransferItem mkSep(const QString& groupName = QString()) {
    return { groupName, QString(), true };
}

// ─────────────────────────────────────────────────────────────────────
//  Static database
// ─────────────────────────────────────────────────────────────────────
class NuclearTransferItemDB {
public:

    // ── SENTINEL used in combobox as last item ────────────────────────
    static constexpr const char* CUSTOM_TEXT = "✏  Custom description...";

    /**
     * Returns predefined input (receiving) transfer items for the given
     * material type.  The CUSTOM_TEXT sentinel is always appended last.
     */
    static QVector<TransferItem> inputItems(const QString& mat)
    {
        const QString m = mat.toLower();
        QVector<TransferItem> v;

        // ── Group: Generic / Common to all materials ──────────────────
        v << mkSep("── Generic Receipts ──");
        v << TransferItem{"Nuclear Material Transfer-In (Generic)",
                          "INFCIRC/153 §30(b): all nuclear material received during the MBP", false};
        v << TransferItem{"Storage Vault Withdrawal",
                          "Material drawn from an approved nuclear material store", false};
        v << TransferItem{"Returned Material (Loan Repayment)",
                          "INFCIRC/153 §35: material returned after a loan or transfer", false};
        v << TransferItem{"Recovered Scrap (In-process Return)",
                          "NMA Handbook §3.2: recovered hold-up or residue counted as input", false};
        v << TransferItem{"Sample Material Return (Post-analysis)",
                          "Sample returned after non-destructive or partial analysis", false};

        // ── Group: Material-specific receipts ─────────────────────────
        if (m.contains("uf6")) {
            v << mkSep("── UF6-Specific Receipts ──");
            v << TransferItem{"Natural UF6 Feed Cylinder Receipt (KMP-A)",
                              "Gross–tare weighed cylinder at KMP-A per IAEA design information", false};
            v << TransferItem{"Enriched UF6 Product Cylinder Receipt",
                              "Enriched UF6 transferred in from another cascade or facility", false};
            v << TransferItem{"Recycled UF6 Process Stream",
                              "Re-enriched or recovered UF6 returned to process", false};
            v << TransferItem{"Depleted UF6 Cylinder Re-Feed",
                              "DU tails cylinder returned for re-enrichment or blending", false};
            v << TransferItem{"UF6 Cascade Feed (Re-enrichment Input)",
                              "Feed to cascade from intermediate storage or blending", false};
            v << TransferItem{"UF6 Tail-gas Recovery (Cylinder Heel)",
                              "Recovered UF6 from cylinder heel or trap system", false};
            v << TransferItem{"UF6 from De-inventorying Operation",
                              "Material recovered during scheduled cascade de-inventory", false};
        }
        else if (m.contains("uo2")) {
            v << mkSep("── UO2-Specific Receipts ──");
            v << TransferItem{"UO2 Powder Batch Receipt",
                              "Lot of UO2 powder received for pelletizing / sintering", false};
            v << TransferItem{"UO2 Pellet Batch Receipt",
                              "Pre-fabricated pellets received for fuel rod loading", false};
            v << TransferItem{"Uranium Ore Concentrate (UOC / Yellowcake) Receipt",
                              "UO3·xH2O or ammonium diuranate received from mine/mill", false};
            v << TransferItem{"UO2 Scrap Return from Fabrication",
                              "Off-spec or recycled UO2 returned for re-processing", false};
            v << TransferItem{"UO3 Batch (for UO2 Reduction)",
                              "UO3 received for conversion to UO2 by hydrogen reduction", false};
            v << TransferItem{"UO2 from Spent Fuel Reprocessing",
                              "Recovered uranium oxide stream from PUREX or equivalent process", false};
        }
        else if (m.contains("leu") || (m.contains("low enriched"))) {
            v << mkSep("── LEU-Specific Receipts ──");
            v << TransferItem{"LEU Solution Receipt (Uranyl Nitrate)",
                              "Aqueous LEU solution transferred in for further processing", false};
            v << TransferItem{"LEU Powder Batch Transfer-In",
                              "LEU powder lot received for pellet fabrication", false};
            v << TransferItem{"LEU Fuel Pellet Receipt",
                              "Finished pellets received for fuel rod assembly", false};
            v << TransferItem{"LEU Fuel Assembly Receipt",
                              "Complete fuel assembly transferred in from fabricator", false};
            v << TransferItem{"Downblended HEU Product (LEU)",
                              "HEU downblended with natural/depleted U to produce LEU", false};
            v << TransferItem{"LEU from Enrichment Cascade (Product Stream)",
                              "Product stream drawn from enrichment plant cascade", false};
        }
        else if (m.contains("heu") || m.contains("highly enriched")) {
            v << mkSep("── HEU-Specific Receipts ──");
            v << TransferItem{"HEU Metal Batch Receipt",
                              "Highly enriched uranium metal ingot received under strict controls", false};
            v << TransferItem{"HEU Solution Transfer-In (Uranyl Nitrate)",
                              "Aqueous HEU solution for processing or storage", false};
            v << TransferItem{"HEU Oxide (UO2 / UO3) Batch Receipt",
                              "HEU oxide powder received for further processing", false};
            v << TransferItem{"HEU from Downblend Feed Receipt",
                              "HEU brought in as feed material for dilution to LEU", false};
            v << TransferItem{"Irradiated HEU Target Return",
                              "Irradiated research reactor target returned for Pu-238 or isotope production", false};
        }
        else if (m.contains("natural uranium")) {
            v << mkSep("── Natural Uranium Receipts ──");
            v << TransferItem{"Uranium Ore Concentrate (UOC / Yellowcake) Receipt",
                              "Primary mine output received at conversion facility (KMP-A)", false};
            v << TransferItem{"Natural Uranium Metal Ingot Batch",
                              "Metallic natural uranium received for machining or irradiation", false};
            v << TransferItem{"Natural Uranium Hexafluoride (UF6) Cylinder Receipt",
                              "Natural UF6 cylinder delivered as enrichment feed", false};
            v << TransferItem{"Natural Uranium Tetrafluoride (UF4) Batch",
                              "UF4 intermediate received from conversion facility", false};
            v << TransferItem{"Natural Uranium Solution Receipt",
                              "Aqueous uranium solution received from dissolution", false};
            v << TransferItem{"Uranium Trioxide (UO3) Batch Receipt",
                              "UO3 calcined from ADU or AUC for UO2 reduction", false};
        }
        else if (m.contains("depleted")) {
            v << mkSep("── Depleted Uranium Receipts ──");
            v << TransferItem{"Depleted UF6 Tails Cylinder Delivery (KMP-D)",
                              "DU tails from enrichment cascade received at storage (KMP-D)", false};
            v << TransferItem{"Depleted UO2 Batch Receipt",
                              "DU oxide received for shielding pellet or ballast production", false};
            v << TransferItem{"Depleted Uranium Metal Batch Receipt",
                              "DU metal for armour, counterweights, or radiation shielding", false};
            v << TransferItem{"DU from Enrichment Cascade (Tails Stream)",
                              "Tails drawn directly from cascade withdrawal point", false};
            v << TransferItem{"DU from Conversion Facility",
                              "Converted DU compound received from fluorination plant", false};
        }
        else if (m.contains("plutonium") || (m.contains("pu") && !m.contains("mox"))) {
            v << mkSep("── Plutonium Receipts ──");
            v << TransferItem{"Pu Nitrate Solution Receipt (PUREX Product)",
                              "Plutonium nitrate solution from reprocessing; high-safeguards item", false};
            v << TransferItem{"PuO2 Powder Batch Receipt",
                              "Calcined plutonium dioxide received in glovebox containers", false};
            v << TransferItem{"Pu Metal Ingot Receipt",
                              "Plutonium metal for research or weapons-material disposition", false};
            v << TransferItem{"Plutonium from Spent Fuel Reprocessing (PUREX / COEX)",
                              "Pu stream from spent fuel reprocessing plant", false};
            v << TransferItem{"MOX Recycle Pu Receipt",
                              "Plutonium recovered from MOX fabrication scrap", false};
            v << TransferItem{"Pu Return from Research Reactor Target",
                              "Pu extracted from irradiated targets (research)", false};
        }
        else if (m.contains("mox")) {
            v << mkSep("── MOX-Specific Receipts ──");
            v << TransferItem{"PuO2 / UO2 Mixed Powder Receipt (MOX Feed)",
                              "Feed powders for MOX pellet production", false};
            v << TransferItem{"MOX Powder Blend Batch Receipt",
                              "Pre-blended MOX powder received for pelletizing", false};
            v << TransferItem{"MOX Fuel Pellet Batch Receipt",
                              "MOX pellets received for fuel rod loading", false};
            v << TransferItem{"MOX Fuel Assembly Receipt",
                              "Complete MOX assembly transferred in from fabricator", false};
            v << TransferItem{"MOX Scrap Return (Recycled)",
                              "Off-spec MOX material returned for re-blend", false};
        }
        else if (m.contains("u-233") || m.contains("u233")) {
            v << mkSep("── U-233 Receipts ──");
            v << TransferItem{"U-233 Solution Receipt",
                              "Aqueous U-233 solution from Th irradiation target processing", false};
            v << TransferItem{"U-233 Oxide Batch Receipt",
                              "U-233 oxide powder received for research reactor fuel", false};
            v << TransferItem{"Irradiated Th Target (U-233 Precursor)",
                              "Post-irradiated Th target containing bred U-233", false};
        }
        else if (m.contains("thorium") || m.contains("th")) {
            v << mkSep("── Thorium Receipts ──");
            v << TransferItem{"ThO2 Powder Batch Receipt",
                              "Thorium dioxide received from ore processing", false};
            v << TransferItem{"Thorium Metal Batch Receipt",
                              "Metallic thorium for reactor fuel or research", false};
            v << TransferItem{"Thorium Nitrate Solution Receipt",
                              "Th(NO3)4 solution for further processing or reactor use", false};
            v << TransferItem{"Thorium Ore Concentrate Receipt",
                              "Monazite or other Th ore concentrate", false};
            v << TransferItem{"Irradiated Th Target Return (Post-irradiation)",
                              "Th target returned after neutron irradiation for U-233 recovery", false};
        }

        // ── Custom entry sentinel ─────────────────────────────────────
        v << mkSep(QString());
        v << TransferItem{QString::fromUtf8(CUSTOM_TEXT),
                          "Select to enter a custom transfer description not listed above", false};
        return v;
    }

    // ─────────────────────────────────────────────────────────────────
    /**
     * Returns predefined output (distributing) transfer items for the
     * given material type.
     */
    static QVector<TransferItem> outputItems(const QString& mat)
    {
        const QString m = mat.toLower();
        QVector<TransferItem> v;

        // ── Group: Generic / Common outputs ───────────────────────────
        v << mkSep("── Generic Distributions ──");
        v << TransferItem{"Nuclear Material Product Shipment (Generic)",
                          "INFCIRC/153 §30(c): all nuclear material shipped during MBP", false};
        v << TransferItem{"Transfer to Nuclear Material Storage/Vault",
                          "Material placed in approved long-term or interim storage", false};
        v << TransferItem{"Sample Withdrawal (Destructive Analysis, DA)",
                          "Sample removed for certified laboratory analysis; counted as output", false};
        v << TransferItem{"Process Hold-Up / In-Process Inventory (End of Period)",
                          "NMA Handbook §2.3: hold-up measured at end of MBP and counted as output", false};
        v << TransferItem{"Waste Stream Transfer (Measured)",
                          "Radioactive waste batch transferred with measured nuclear content", false};
        v << TransferItem{"Material Sent on Loan to Another Facility",
                          "INFCIRC/153 §35: loan transfer; receiver becomes shipper of record", false};
        v << TransferItem{"Discard / Disposal (State-Authorized)",
                          "Authorised write-off or disposal below measurement threshold", false};
        v << TransferItem{"Material for Off-site Measurement / Verification",
                          "Sample or item sent for IAEA or operator verification measurement", false};

        // ── Material-specific outputs ──────────────────────────────────
        if (m.contains("uf6")) {
            v << mkSep("── UF6-Specific Distributions ──");
            v << TransferItem{"Enriched UF6 Product Cylinder Shipment",
                              "Product cylinder dispatched to converter or fuel fabricator (KMP-B)", false};
            v << TransferItem{"Depleted UF6 Tails Cylinder Transfer (KMP-D)",
                              "Tails cylinder moved to DU storage or cylinder yard", false};
            v << TransferItem{"UF6 Cascade Process Hold-Up (Measured)",
                              "UF6 retained in cascade piping, traps, and cold traps at period end", false};
            v << TransferItem{"UF6 Sample Cylinder Withdrawal (DA)",
                              "Small cylinder drawn for isotopic / uranium content analysis", false};
            v << TransferItem{"UF6 Cylinder Maintenance Residue",
                              "Residual UF6 from cylinder cleaning or maintenance operations", false};
            v << TransferItem{"UF6 Gaseous Effluent (Process Loss, Measured)",
                              "Measured UF6 released to scrubber / effluent system", false};
            v << TransferItem{"Off-spec UF6 Cylinder (Returned or Downgraded)",
                              "UF6 below specification transferred for re-processing", false};
        }
        else if (m.contains("uo2")) {
            v << mkSep("── UO2-Specific Distributions ──");
            v << TransferItem{"UO2 Fuel Pellet Product Shipment",
                              "Finished pellet lot dispatched to fuel rod assembly", false};
            v << TransferItem{"UO2 Fuel Assembly Shipment to Reactor",
                              "Complete fuel assembly delivered to reactor facility (KMP)", false};
            v << TransferItem{"UO2 Scrap Transfer (Off-spec or Recycled)",
                              "UO2 powder or pellet scrap transferred for recovery", false};
            v << TransferItem{"UO2 Waste Powder / Sintering Loss (Measured)",
                              "Process loss from sintering measured and declared as output", false};
            v << TransferItem{"Leached Hulls and End Caps (Post-reprocessing)",
                              "Cladding hulls with residual uranium content after leaching", false};
            v << TransferItem{"UO2 Solution Discharge (Dissolution Raffinate)",
                              "Uranium-bearing raffinate from dissolution step", false};
        }
        else if (m.contains("leu") || m.contains("low enriched")) {
            v << mkSep("── LEU-Specific Distributions ──");
            v << TransferItem{"LEU Product Shipment (Powder / Solution)",
                              "LEU dispatched to fuel fabrication or conversion facility", false};
            v << TransferItem{"LEU Fuel Assembly Shipment",
                              "Complete LEU assembly delivered to reactor", false};
            v << TransferItem{"LEU In-Process Hold (Cascade / Line)",
                              "LEU retained in process equipment at period-end (hold-up)", false};
            v << TransferItem{"LEU Scrap / Residue Transfer",
                              "Off-spec LEU or contaminated scrap transferred for recovery", false};
            v << TransferItem{"LEU Sample Withdrawal (Isotopic / DA)",
                              "Sample for enrichment verification and uranium assay", false};
        }
        else if (m.contains("heu") || m.contains("highly enriched")) {
            v << mkSep("── HEU-Specific Distributions ──");
            v << TransferItem{"HEU Product Transfer (Metal / Oxide)",
                              "HEU transferred under strict inventory control and escort", false};
            v << TransferItem{"HEU Downblend Feed Transfer (to Dilution Plant)",
                              "HEU material sent for dilution with natural/DU to produce LEU", false};
            v << TransferItem{"HEU Process Waste / Residue (Measured)",
                              "Measured HEU waste from machining, dissolution, or conversion", false};
            v << TransferItem{"HEU Sample Withdrawal (DA)",
                              "Sample for criticality safety and isotopic verification", false};
            v << TransferItem{"HEU Material to Long-term Secure Storage",
                              "Transfer to approved vault pending disposition", false};
        }
        else if (m.contains("natural uranium")) {
            v << mkSep("── Natural Uranium Distributions ──");
            v << TransferItem{"Natural UF6 Feed Cylinder Shipment (to Enrichment)",
                              "Natural UF6 dispatched as feed to enrichment facility", false};
            v << TransferItem{"UO3 / ADU Product Transfer (Conversion Output)",
                              "Calcined product transferred from conversion plant", false};
            v << TransferItem{"UF4 Intermediate Product Transfer",
                              "UF4 dispatched to fluorination or storage", false};
            v << TransferItem{"Natural Uranium Solution Discharge (UNH)",
                              "Uranyl nitrate solution from dissolution step", false};
            v << TransferItem{"Natural U Process Residue / Scrap Transfer",
                              "Uranium-bearing residues from conversion operations", false};
        }
        else if (m.contains("depleted")) {
            v << mkSep("── Depleted Uranium Distributions ──");
            v << TransferItem{"Depleted UF6 Tails Cylinder to Long-term Storage",
                              "DU tails cylinder transferred to approved cylinder yard", false};
            v << TransferItem{"DU Product Shipment (Shielding / Counterweights)",
                              "Processed DU product for authorised non-nuclear use", false};
            v << TransferItem{"DU Scrap / Metal Transfer (Recycling)",
                              "DU metal scrap transferred for recovery or re-melt", false};
            v << TransferItem{"DU Conversion Product (DUO2 / DUF4) Shipment",
                              "Converted DU compound dispatched from deconversion plant", false};
            v << TransferItem{"DU Waste Disposal (Authorised)",
                              "DU material cleared for disposal under national regulations", false};
        }
        else if (m.contains("plutonium") || (m.contains("pu") && !m.contains("mox"))) {
            v << mkSep("── Plutonium Distributions ──");
            v << TransferItem{"Pu Product Transfer to MOX Fabrication Plant",
                              "PuO2 or Pu solution dispatched for MOX fuel production", false};
            v << TransferItem{"PuO2 Powder Product Shipment",
                              "Calcined PuO2 dispatched under high-security transport", false};
            v << TransferItem{"Pu Scrap / Residue Transfer (Recovery)",
                              "Pu-bearing scrap sent for recovery operations", false};
            v << TransferItem{"Pu Sample Withdrawal (Isotopic DA)",
                              "Sample for isotopic composition and mass verification", false};
            v << TransferItem{"Pu Process Waste Transfer (Measured)",
                              "Waste streams with measured Pu content", false};
            v << TransferItem{"Pu to Interim Storage / Repository",
                              "Pu material placed in approved interim or long-term storage", false};
        }
        else if (m.contains("mox")) {
            v << mkSep("── MOX-Specific Distributions ──");
            v << TransferItem{"MOX Fuel Assembly Shipment to Reactor",
                              "Finished MOX assembly delivered to reactor (KMP)", false};
            v << TransferItem{"MOX Powder Scrap Transfer (Off-spec)",
                              "MOX powder below specification sent for recovery", false};
            v << TransferItem{"MOX Off-spec Waste Transfer (Measured)",
                              "Contaminated or off-spec waste with measured nuclear content", false};
            v << TransferItem{"MOX Sample Withdrawal (DA/NDA)",
                              "Sample for Pu/U ratio and enrichment verification", false};
            v << TransferItem{"MOX In-process Hold-up (End of Period)",
                              "Material measured in production line at period-end", false};
        }
        else if (m.contains("u-233") || m.contains("u233")) {
            v << mkSep("── U-233 Distributions ──");
            v << TransferItem{"U-233 Product Transfer (Solution / Oxide)",
                              "U-233 dispatched for research reactor fuel or irradiation", false};
            v << TransferItem{"U-233 Waste / Residue Transfer (Measured)",
                              "Waste streams with measured U-233 content", false};
            v << TransferItem{"U-233 Sample Withdrawal (DA)",
                              "Sample for isotopic purity and mass verification", false};
        }
        else if (m.contains("thorium") || m.contains("th")) {
            v << mkSep("── Thorium Distributions ──");
            v << TransferItem{"ThO2 Product Shipment",
                              "Thorium dioxide dispatched to fuel fabricator or reactor", false};
            v << TransferItem{"Thorium Metal Shipment",
                              "Metallic Th dispatched for research or reactor use", false};
            v << TransferItem{"Irradiated Th Target Transfer (to Reprocessing)",
                              "Post-irradiation Th target sent for U-233 extraction", false};
            v << TransferItem{"Thorium Process Waste Transfer (Measured)",
                              "Measured waste from conversion or machining operations", false};
        }

        // ── Custom entry sentinel ─────────────────────────────────────
        v << mkSep(QString());
        v << TransferItem{QString::fromUtf8(CUSTOM_TEXT),
                          "Select to enter a custom transfer description not listed above", false};
        return v;
    }

    // ─────────────────────────────────────────────────────────────────
    /**
     * Returns Physical Inventory description items for the Beginning
     * Inventory (isOpening = true) or Ending Inventory (isOpening = false)
     * steps.  Items reflect the type of physical stock measurement at
     * period open/close per IAEA NMA Handbook §2.2 and INFCIRC/153 §3.
     */
    static QVector<TransferItem> inventoryItems(const QString& mat, bool isOpening)
    {
        const QString m = mat.toLower();
        QVector<TransferItem> v;

        // ── Generic items applicable to all material types ────────────
        if (isOpening) {
            v << mkSep("── Opening Physical Inventory ──");
            v << TransferItem{"Physical Inventory Verification (PIV) Opening Stock",
                              "IAEA NMA Handbook §2.2: physically verified inventory closing the previous MBP", false};
            v << TransferItem{"Beginning of Period Physical Inventory (KMP Measured)",
                              "INFCIRC/153 §3.1: inventory measured at KMP at period start", false};
            v << TransferItem{"Storage Facility Opening Physical Count",
                              "Total measured stock in approved nuclear material storage at period start", false};
            v << TransferItem{"Carry-Forward Inventory from Previous MBP",
                              "Closing inventory of the immediately preceding Material Balance Period", false};
            v << TransferItem{"Initial Facility Loading (First MBP)",
                              "Fresh material loaded at facility commissioning — first balance period only", false};
        } else {
            v << mkSep("── Closing Physical Inventory ──");
            v << TransferItem{"Physical Inventory Verification (PIV) Closing Stock",
                              "IAEA NMA Handbook §2.2: physically verified inventory closing the current MBP", false};
            v << TransferItem{"End of Period Physical Inventory (KMP Measured)",
                              "INFCIRC/153 §3.1: inventory measured at KMP at period close", false};
            v << TransferItem{"Storage Facility Closing Physical Count",
                              "Total measured stock in approved nuclear material storage at period end", false};
            v << TransferItem{"Physical Inventory after Maintenance Shutdown",
                              "Inventory re-measured following facility maintenance outage", false};
        }

        // ── Generic process inventory (both directions) ───────────────
        v << mkSep("── In-Process Inventory ──");
        v << TransferItem{"In-Process Inventory (Measured Hold-Up)",
                          "NMA Handbook §3.2: nuclear material retained in process equipment — measured", false};
        v << TransferItem{"Process Line Inventory (Cascade / Conversion)",
                          "Material held in active process piping, vessels, and traps at measurement time", false};
        v << TransferItem{"Scrap and Residue Inventory (Measured)",
                          "Measured nuclear material in scrap, residue containers, or waste pending disposition", false};
        v << TransferItem{"Sample Archive Inventory",
                          "Retained sample material from DA operations — counted in physical inventory", false};

        // ── Material-specific inventory items ─────────────────────────
        if (m.contains("uf6")) {
            v << mkSep("── UF6 Physical Inventory ──");
            v << TransferItem{"UF6 Feed Cylinder Stock (KMP-A)",
                              "Weighed natural or low-enriched UF6 feed cylinders at cylinder yard (KMP-A)", false};
            v << TransferItem{"Enriched UF6 Product Cylinder Stock (KMP-B)",
                              "Weighed enriched UF6 product cylinders awaiting dispatch (KMP-B)", false};
            v << TransferItem{"Depleted UF6 Tails Cylinder Stock (KMP-D)",
                              "Weighed DU tails cylinders in cylinder storage yard (KMP-D)", false};
            v << TransferItem{"Cascade UF6 Process Hold-Up (Measured)",
                              "UF6 retained in cascade piping, cold traps, and compressors — measured at period boundary", false};
            v << TransferItem{"UF6 Maintenance Hold-Up Inventory",
                              "UF6 residue in equipment removed for maintenance — weighed and counted", false};
        }
        else if (m.contains("uo2")) {
            v << mkSep("── UO2 Physical Inventory ──");
            v << TransferItem{"UO2 Powder Lot Inventory (Measured)",
                              "Weighed UO2 powder in certified containers at period boundary", false};
            v << TransferItem{"UO2 Pellet Batch Inventory (Measured)",
                              "Weighed pellet lots in certified trays or containers", false};
            v << TransferItem{"Fuel Rod / Assembly Store Inventory",
                              "Measured UO2 content of assembled fuel rods or partial assemblies in store", false};
            v << TransferItem{"UO2 Sintering Scrap Inventory",
                              "Measured scrap powder and off-spec pellets from sintering operations", false};
        }
        else if (m.contains("leu") || m.contains("low enriched")) {
            v << mkSep("── LEU Physical Inventory ──");
            v << TransferItem{"LEU Solution Inventory (Measured)",
                              "Weighed or volumetrically measured LEU solution in certified tanks", false};
            v << TransferItem{"LEU Fuel Assembly Store Inventory",
                              "Measured LEU content of assemblies in reactor fuel store", false};
            v << TransferItem{"LEU Powder and Pellet Inventory (Measured)",
                              "Weighed LEU powder or pellet lots in certified containers", false};
            v << TransferItem{"LEU In-process Hold-Up (Measured)",
                              "Measured LEU retained in active production line at period boundary", false};
        }
        else if (m.contains("heu") || m.contains("highly enriched")) {
            v << mkSep("── HEU Physical Inventory ──");
            v << TransferItem{"HEU Vault Inventory (Measured)",
                              "Weighed HEU items in approved vault storage — each item individually measured", false};
            v << TransferItem{"HEU Metal Lot Inventory",
                              "Weighed HEU metal billets, buttons, or machined components", false};
            v << TransferItem{"HEU Solution Inventory (Measured)",
                              "Volumetrically or gravimetrically measured HEU solution in certified vessels", false};
            v << TransferItem{"HEU In-Process Inventory (Measured)",
                              "Measured HEU in active process equipment at period boundary — high-priority item", false};
        }
        else if (m.contains("natural uranium")) {
            v << mkSep("── Natural Uranium Inventory ──");
            v << TransferItem{"Natural Uranium Drum / Drum-Lot Inventory",
                              "Weighed drums of UO3, UF4, or natural uranium metal at period boundary", false};
            v << TransferItem{"UF6 Feed Cylinder Inventory (Natural)",
                              "Weighed natural UF6 cylinders in cylinder yard or storage pad", false};
            v << TransferItem{"Ore Concentrate (UOC / Yellowcake) Inventory",
                              "Measured uranium ore concentrate stock at conversion facility", false};
            v << TransferItem{"Natural Uranium Solution Inventory",
                              "Volumetrically measured uranium-bearing solution stock", false};
        }
        else if (m.contains("depleted")) {
            v << mkSep("── Depleted Uranium Inventory ──");
            v << TransferItem{"Depleted UF6 Tails Cylinder Inventory (KMP-D)",
                              "Weighed DU tails cylinders in cylinder yard — gross minus tare", false};
            v << TransferItem{"Depleted UO2 Inventory (Shielding / Ballast)",
                              "Weighed DU oxide product in authorised storage", false};
            v << TransferItem{"Depleted Uranium Metal Inventory",
                              "Weighed DU metal items (counterweights, armour billets) in storage", false};
            v << TransferItem{"DU Conversion Product Inventory",
                              "Measured DU compound stock at deconversion or fluorination facility", false};
        }
        else if (m.contains("plutonium") || (m.contains("pu") && !m.contains("mox"))) {
            v << mkSep("── Plutonium Physical Inventory ──");
            v << TransferItem{"Pu Vault Inventory (Glovebox Measured)",
                              "Individually weighed PuO2 cans or Pu metal items in approved vault", false};
            v << TransferItem{"Pu Solution Inventory (Measured)",
                              "Measured plutonium nitrate solution in certified process vessels", false};
            v << TransferItem{"Pu Scrap and Residue Inventory (Measured)",
                              "Measured Pu content of scrap, residues, and waste containers in store", false};
            v << TransferItem{"Pu In-Process Hold-Up (Measured)",
                              "Measured Pu in active process equipment at period boundary — highest safeguards priority", false};
        }
        else if (m.contains("mox")) {
            v << mkSep("── MOX Physical Inventory ──");
            v << TransferItem{"MOX Powder Inventory (Measured)",
                              "Weighed MOX powder lots in certified glovebox containers", false};
            v << TransferItem{"MOX Pellet Lot Inventory (Measured)",
                              "Weighed MOX pellet lots in certified trays", false};
            v << TransferItem{"MOX Fuel Assembly Store Inventory",
                              "Measured Pu content of MOX assemblies in reactor fuel store", false};
            v << TransferItem{"MOX In-Process Hold-Up (Measured)",
                              "Measured MOX material in active production line at period boundary", false};
        }
        else if (m.contains("u-233") || m.contains("u233")) {
            v << mkSep("── U-233 Physical Inventory ──");
            v << TransferItem{"U-233 Vault Inventory (Measured)",
                              "Individually weighed U-233 items in approved vault storage", false};
            v << TransferItem{"U-233 Solution Inventory (Measured)",
                              "Measured U-233 solution in certified process vessels", false};
            v << TransferItem{"U-233 Process Inventory (Measured)",
                              "Measured U-233 in active equipment at period boundary — direct-use material", false};
        }
        else if (m.contains("thorium") || m.contains("th")) {
            v << mkSep("── Thorium Physical Inventory ──");
            v << TransferItem{"ThO2 Drum Inventory (Measured)",
                              "Weighed thorium dioxide drums in certified storage", false};
            v << TransferItem{"Thorium Metal Inventory (Measured)",
                              "Weighed thorium metal items in approved store", false};
            v << TransferItem{"Thorium Solution Inventory (Measured)",
                              "Volumetrically measured thorium nitrate solution stock", false};
            v << TransferItem{"Irradiated Th Target Inventory (Post-irradiation)",
                              "Measured Th targets returned from irradiation facility, awaiting reprocessing", false};
        }

        // ── Custom entry sentinel ─────────────────────────────────────
        v << mkSep(QString());
        v << TransferItem{QString::fromUtf8(CUSTOM_TEXT),
                          "Select to enter a custom inventory description not listed above", false};
        return v;
    }

    /**
     * Populates a QComboBox with Physical Inventory description items.
     * Call this for the m_biLabel (isOpening=true) and m_eiLabel
     * (isOpening=false) combos on Pages 2 and 5.
     */
    static void populateInventoryCombo(QComboBox* combo,
                                       const QString& materialType,
                                       bool isOpening,
                                       const QString& preserveText = QString())
    {
        if (!combo) return;
        populateFromItems(combo, inventoryItems(materialType, isOpening), preserveText);
    }

    /**
     * Fills a QComboBox with all items for the given material and
     * direction, using separators as visual group headers.
     * Editable items are set as Qt::ItemIsSelectable | Qt::ItemIsEnabled.
     * Separator rows are set non-selectable and styled.
     */
    static void populateCombo(QComboBox* combo,
                              const QString& materialType,
                              bool isInput,
                              const QString& preserveText = QString())
    {
        if (!combo) return;
        populateFromItems(combo, isInput ? inputItems(materialType)
                                         : outputItems(materialType), preserveText);
    }

private:
    /** Shared helper: fill combo from any QVector<TransferItem>. */
    static void populateFromItems(QComboBox* combo,
                                  const QVector<TransferItem>& items,
                                  const QString& preserveText)
    {
        if (!combo) return;
        combo->blockSignals(true);
        combo->clear();

        for (const auto& item : items) {
            if (item.separator) {
                combo->addItem(item.text);
                int idx = combo->count() - 1;
                if (auto* mdl = qobject_cast<QStandardItemModel*>(combo->model())) {
                    if (auto* si = mdl->item(idx)) {
                        si->setFlags(si->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
                        si->setData(QColor("#6b7280"), Qt::ForegroundRole);
                        QFont f = si->font(); f.setItalic(true); si->setFont(f);
                    }
                }
                combo->setItemData(idx, "separator", Qt::UserRole);
            } else {
                combo->addItem(item.text);
                int idx = combo->count() - 1;
                if (!item.tooltip.isEmpty())
                    combo->setItemData(idx, item.tooltip, Qt::ToolTipRole);
            }
        }

        // Restore previous text if still in list; otherwise use first selectable item
        if (!preserveText.isEmpty()) {
            int found = combo->findText(preserveText);
            if (found >= 0) {
                combo->setCurrentIndex(found);
            } else {
                combo->setCurrentText(preserveText);   // custom text — keep it
            }
        } else {
            for (int i = 0; i < combo->count(); ++i) {
                if (combo->itemData(i, Qt::UserRole).toString() != "separator") {
                    combo->setCurrentIndex(i);
                    break;
                }
            }
        }

        combo->blockSignals(false);
    }
};
