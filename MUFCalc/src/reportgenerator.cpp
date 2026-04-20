#include "reportgenerator.h"
#include <QFile>
#include <QTextStream>
#include <cmath>

QString ReportGenerator::f6(double v) const { return QString::number(v,'f',6); }
QString ReportGenerator::f4(double v) const { return QString::number(v,'f',4); }

QString ReportGenerator::severityStr(Severity s) const {
    switch(s){ case Severity::OK: return "OK"; case Severity::INFO: return "INFO";
               case Severity::WARNING: return "WARNING"; case Severity::ALERT: return "ALERT";
               case Severity::CRITICAL: return "CRITICAL"; }
    return "?";
}
QString ReportGenerator::severityColor(Severity s) const {
    switch(s){ case Severity::OK:       return "#22c55e";
               case Severity::INFO:     return "#3b82f6";
               case Severity::WARNING:  return "#f59e0b";
               case Severity::ALERT:    return "#f97316";
               case Severity::CRITICAL: return "#ef4444"; }
    return "#888";
}

QString ReportGenerator::generateHTML(const MBPInput& in, const MUFResult& r) const {
    double ratio = r.sigmaMUF > 0 ? std::abs(r.muf)/r.sigmaMUF : 0;
    QString statusCol = ratio<=1?"#22c55e":ratio<=2?"#f59e0b":ratio<=3?"#f97316":"#ef4444";
    QString statusTxt = ratio<=1?"✔ NORMAL (≤1σ)":ratio<=2?"⚠ ELEVATED (1σ-2σ)":
                        ratio<=3?"⚠ WARNING (2σ-3σ)":"✖ CRITICAL (>3σ)";
    QString html = R"(<!DOCTYPE html>
<html lang="en"><head><meta charset="UTF-8">
<title>MUF Calculation Report</title>
<style>
body{font-family:'Segoe UI',sans-serif;background:#05080f;color:#e2e8f0;margin:0;padding:20px}
.card{background:#0b1120;border:1px solid #1e293b;border-radius:10px;padding:20px;margin-bottom:16px}
h1{font-size:27px;color:#93c5fd;margin:0 0 4px}.sub{color:#111827;font-size:18px;margin-bottom:20px}
h2{color:#93c5fd;font-size:20px;border-bottom:1px solid #1e293b;padding-bottom:8px;margin-top:0}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.stat{background:#080d18;border-radius:8px;padding:12px 16px}
.sv{font-size:27px;font-weight:900;font-family:monospace}.sl{color:#111827;font-size:16px;margin-top:2px}
.eq{background:#080d18;border-left:3px solid #2563eb;padding:12px 16px;border-radius:0 6px 6px 0;font-family:monospace;font-size:18px;color:#93c5fd;margin:8px 0;line-height:2}
.flag{padding:12px 16px;border-radius:6px;margin-bottom:8px}
table{width:100%;border-collapse:collapse;font-size:18px}
th{background:#080d18;color:#111827;padding:8px 12px;text-align:left;border-bottom:1px solid #1e293b;font-size:16px;text-transform:uppercase;letter-spacing:1px}
td{padding:7px 12px;border-bottom:1px solid #080d18}
.bar-bg{background:#1e293b;border-radius:6px;height:10px;margin-top:4px}
.bar-fill{height:10px;border-radius:6px}
footer{color:#374151;font-size:16px;text-align:center;margin-top:30px;font-family:monospace}
</style></head><body>)";

    html += QString("<div style='background:#0d1530;border-radius:10px;padding:20px 24px;margin-bottom:20px'>"
                    "<h1>⚛ MUF Calculation Report</h1>"
                    "<div class='sub'>IAEA Safeguards Compliant &bull; Generated: %1</div>"
                    "</div>").arg(r.calculatedAt.toString("yyyy-MM-dd HH:mm:ss"));

    // Facility
    html += QString("<div class='card'><h2>Facility &amp; Period</h2><div class='grid'>"
        "<div class='stat'><div class='sl'>FACILITY NAME</div><div style='color:#e2e8f0;font-weight:600'>%1</div></div>"
        "<div class='stat'><div class='sl'>FACILITY ID</div><div style='color:#e2e8f0;font-weight:600'>%2</div></div>"
        "<div class='stat'><div class='sl'>MATERIAL TYPE</div><div style='color:#e2e8f0;font-weight:600'>%3</div></div>"
        "<div class='stat'><div class='sl'>MBP ID</div><div style='color:#e2e8f0;font-weight:600'>%4</div></div>"
        "<div class='stat'><div class='sl'>PERIOD START</div><div style='color:#e2e8f0;font-weight:600'>%5</div></div>"
        "<div class='stat'><div class='sl'>PERIOD END</div><div style='color:#e2e8f0;font-weight:600'>%6</div></div>"
        "</div></div>")
        .arg(in.facilityName, in.facilityId, in.materialType, in.mbpId,
             in.periodStart.toString("yyyy-MM-dd"), in.periodEnd.toString("yyyy-MM-dd"));

    // MUF banner
    html += QString("<div class='card' style='background:%1;border-color:%2'>"
        "<div style='text-align:center;padding:16px 0'>"
        "<div style='color:#111827;font-size:18px;margin-bottom:6px'>MATERIAL UNACCOUNTED FOR  &bull;  MUF = IB &minus; EI</div>"
        "<div style='font-size:54px;font-weight:900;color:%2;font-family:monospace'>%3%4 kg</div>"
        "<div style='display:inline-block;margin-top:12px;padding:6px 20px;background:%2;color:#000;font-weight:900;border-radius:20px;font-size:18px'>%5</div>"
        "</div></div>")
        .arg(ratio<=1?"#052e16":ratio<=2?"#2d1a00":ratio<=3?"#2d0e00":"#2d0000")
        .arg(statusCol)
        .arg(r.muf>=0?"+":"")
        .arg(f6(r.muf))
        .arg(statusTxt);

    // Stat grid
    html += "<div class='card'><h2>Key Metrics</h2><div class='grid'>";
    auto stat = [&](const QString& lbl, const QString& val, const QString& col){
        return QString("<div class='stat'><div class='sv' style='color:%3'>%2</div><div class='sl'>%1</div></div>")
                   .arg(lbl,val,col);
    };
    html += stat("σ(MUF)", "±"+f6(r.sigmaMUF)+" kg", "#c084fc");
    html += stat("|MUF|/σ RATIO", QString::number(ratio,'f',3)+"σ", statusCol);
    html += stat("BOOK INVENTORY (IB)", f4(r.bookInventory)+" kg", "#38bdf8");
    html += stat("ΣInputs", f4(r.sumInputs)+" kg", "#34d399");
    html += stat("ΣOutputs", f4(r.sumOutputs)+" kg", "#f87171");
    html += stat("ENDING INVENTORY (EI)", f4(in.endingInventory.qty)+" kg", "#fbbf24");
    html += "</div></div>";

    // Equations
    html += "<div class='card'><h2>IAEA Equations — Step by Step</h2>";
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 1 — BOOK INVENTORY</p>"
                    "<div class='eq'>IB = BI + &Sigma;Inputs &minus; &Sigma;Outputs<br/>"
                    "IB = %1 + %2 &minus; %3<br/>"
                    "<span style='color:#4ade80;font-weight:700'>IB = %4 kg</span></div>")
                .arg(f4(in.beginningInventory.qty), f4(r.sumInputs), f4(r.sumOutputs), f6(r.bookInventory));
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 2 — MUF</p>"
                    "<div class='eq'>MUF = IB &minus; EI<br/>"
                    "MUF = %1 &minus; %2<br/>"
                    "<span style='color:#4ade80;font-weight:700'>MUF = %3 kg</span></div>")
                .arg(f6(r.bookInventory), f6(in.endingInventory.qty), f6(r.muf));
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 3 — UNCERTAINTY PROPAGATION</p>"
                    "<div class='eq'>&sigma;(MUF) = &radic;[&sigma;&sup2;(BI) + &sigma;&sup2;(&Sigma;In) + &sigma;&sup2;(&Sigma;Out)]<br/>"
                    "&sigma;(MUF) = &radic;[%1 + %2 + %3]<br/>"
                    "<span style='color:#4ade80;font-weight:700'>&sigma;(MUF) = %4 kg</span></div>")
                .arg(f6(r.sigmaBI*r.sigmaBI), f6(r.sigmaInputs*r.sigmaInputs),
                     f6(r.sigmaOutputs*r.sigmaOutputs), f6(r.sigmaMUF));
    html += "</div>";

    // Uncertainty table
    html += "<div class='card'><h2>Uncertainty Contributions  C&#7522; = [&sigma;&sup2;(i)/&sigma;&sup2;(MUF)] &times; 100%</h2>"
            "<table><tr><th>Term</th><th>σ(i) kg</th><th>σ²(i) kg²</th><th>Cᵢ (%)</th><th style='width:200px'>Visual</th></tr>";
    for (auto& c : r.contributions) {
        QString col = c.percentContrib>50?"#ef4444":c.percentContrib>25?"#f59e0b":"#3b82f6";
        html += QString("<tr><td style='color:#e2e8f0'>%1</td>"
                        "<td style='font-family:monospace;color:#93c5fd'>%2</td>"
                        "<td style='font-family:monospace;color:#93c5fd'>%3</td>"
                        "<td style='font-weight:900;color:%4'>%5%</td>"
                        "<td><div class='bar-bg'><div class='bar-fill' style='width:%6%;background:%4'></div></div></td></tr>")
                    .arg(c.label)
                    .arg(f6(std::sqrt(c.varianceContrib)))
                    .arg(f6(c.varianceContrib))
                    .arg(col)
                    .arg(c.percentContrib,0,'f',2)
                    .arg(qMin(100.0,c.percentContrib),0,'f',0);
    }
    html += "</table></div>";

    // Diagnostics
    html += "<div class='card'><h2>Safeguards Diagnostics</h2>";
    for (auto& f : r.flags) {
        QString bg,bc;
        switch(f.severity){
        case Severity::OK:       bg="#052e16"; bc="#16a34a"; break;
        case Severity::INFO:     bg="#0c1a3a"; bc="#2563eb"; break;
        case Severity::WARNING:  bg="#2d1a00"; bc="#d97706"; break;
        case Severity::ALERT:    bg="#2d0e00"; bc="#ea580c"; break;
        case Severity::CRITICAL: bg="#2d0000"; bc="#dc2626"; break;
        }
        html += QString("<div class='flag' style='background:%1;border-left:3px solid %2'>"
                        "<span style='background:%2;color:#000;font-size:12px;font-weight:900;padding:2px 8px;border-radius:4px'>%3</span>"
                        "&nbsp;<code style='color:%2;font-weight:700'>%4</code>"
                        "<p style='color:#d1d5db;font-size:18px;margin:6px 0 4px'>%5</p>"
                        "<p style='color:#111827;font-size:17px;margin:0'>&#9658; %6</p>"
                        "</div>")
                    .arg(bg,bc, severityStr(f.severity), f.code, f.message, f.action);
    }
    html += "</div>";

    html += "<footer>MUF CALCULATOR v1.0 &bull; IAEA SAFEGUARDS COMPLIANT &bull; OPEN SOURCE "
            "&bull; SAMUEL NARTEY TEYE &bull; TOMSK 2025 &bull; FOR EDUCATIONAL USE ONLY</footer>"
            "</body></html>";
    return html;
}

QString ReportGenerator::generatePlainText(const MBPInput& in, const MUFResult& r) const {
    QString txt;
    QTextStream s(&txt);
    s << "================================================================\n"
      << "  MUF CALCULATOR v1.0 — IAEA SAFEGUARDS COMPLIANT REPORT\n"
      << "================================================================\n"
      << "Generated   : " << r.calculatedAt.toString("yyyy-MM-dd HH:mm:ss") << "\n"
      << "Facility    : " << in.facilityName << " [" << in.facilityId << "]\n"
      << "Material    : " << in.materialType << "\n"
      << "MBP ID      : " << in.mbpId << "\n"
      << "Period      : " << in.periodStart.toString("yyyy-MM-dd")
                          << " to " << in.periodEnd.toString("yyyy-MM-dd") << "\n"
      << "Operator    : " << in.operatorName << "\n\n"
      << "----------------------------------------------------------------\n"
      << "  IAEA EQUATIONS\n"
      << "----------------------------------------------------------------\n"
      << "  BI     = " << f4(in.beginningInventory.qty) << " ± " << f4(in.beginningInventory.error) << " kg\n"
      << "  ΣIn    = " << f4(r.sumInputs)  << " kg\n"
      << "  ΣOut   = " << f4(r.sumOutputs) << " kg\n"
      << "  IB     = BI + ΣIn − ΣOut = " << f6(r.bookInventory) << " kg\n"
      << "  EI     = " << f4(in.endingInventory.qty) << " ± " << f4(in.endingInventory.error) << " kg\n"
      << "  MUF    = IB − EI = " << f6(r.muf) << " kg\n"
      << "  σ²(MUF)= " << f6(r.varianceMUF) << " kg²  (= σ²(BI)+σ²(ΣIn)+σ²(ΣOut))\n"
      << "  σ(MUF) = " << f6(r.sigmaMUF) << " kg\n"
      << "  Ratio  = |MUF|/σ = " << QString::number(r.ratio,'f',3) << " σ\n\n"
      << "----------------------------------------------------------------\n"
      << "  UNCERTAINTY CONTRIBUTIONS\n"
      << "----------------------------------------------------------------\n";
    for (auto& c : r.contributions) {
        // Fixed: Qt positional args, left-pad label to 44 chars for alignment
        QString labelPadded = c.label.leftJustified(44, ' ');
        s << QString("  %1  var=%2 kg²   Ci=%3%\n")
               .arg(labelPadded)
               .arg(f6(c.varianceContrib))
               .arg(c.percentContrib, 6, 'f', 2);
    }
    s << "\n----------------------------------------------------------------\n"
      << "  DIAGNOSTIC FLAGS\n"
      << "----------------------------------------------------------------\n";
    for (auto& f : r.flags) {
        s << "[" << severityStr(f.severity) << "] " << f.code << "\n"
          << "  " << f.message << "\n"
          << "  → " << f.action << "\n\n";
    }
    s << "================================================================\n";
    return txt;
}

bool ReportGenerator::saveHTML(const QString& p, const MBPInput& in, const MUFResult& r) const {
    QFile f(p); if (!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream(&f) << generateHTML(in, r); return true;
}
bool ReportGenerator::savePlainText(const QString& p, const MBPInput& in, const MUFResult& r) const {
    QFile f(p); if (!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream(&f) << generatePlainText(in, r); return true;
}
