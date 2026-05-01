#include "HtmlReport.h"
#include "../utils/FormatUtils.h"
#include <cmath>

namespace MUFCalc {

QString HtmlReport::render(const MufInput& in, const MufResult& r) const {
    using FormatUtils::f4;
    using FormatUtils::f6;
    using FormatUtils::severityLabel;

    const double ratio    = r.sigmaMuf > 0 ? std::abs(r.muf) / r.sigmaMuf : 0.0;
    const QString statusCol = ratio <= 1 ? "#22c55e"
                            : ratio <= 2 ? "#f59e0b"
                            : ratio <= 3 ? "#f97316"
                                          : "#ef4444";
    const QString statusTxt = ratio <= 1 ? "\u2714 NORMAL (\u22641\u03c3)"
                            : ratio <= 2 ? "\u26a0 ELEVATED (1\u03c3-2\u03c3)"
                            : ratio <= 3 ? "\u26a0 WARNING (2\u03c3-3\u03c3)"
                                          : "\u2716 CRITICAL (>3\u03c3)";

    QString html = R"(<!DOCTYPE html>
<html lang="en"><head><meta charset="UTF-8">
<title>MUF Calculation Report</title>
<style>
body{font-family:'Segoe UI',sans-serif;background:#05080f;color:#e2e8f0;margin:0;padding:20px}
.card{background:#0b1120;border:1px solid #1e293b;border-radius:10px;padding:20px;margin-bottom:16px}
h1{font-size:27px;color:#93c5fd;margin:0 0 4px}.sub{color:#94a3b8;font-size:18px;margin-bottom:20px}
h2{color:#93c5fd;font-size:20px;border-bottom:1px solid #1e293b;padding-bottom:8px;margin-top:0}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.stat{background:#080d18;border-radius:8px;padding:12px 16px}
.sv{font-size:27px;font-weight:900;font-family:monospace}.sl{color:#94a3b8;font-size:16px;margin-top:2px}
.eq{background:#080d18;border-left:3px solid #2563eb;padding:12px 16px;border-radius:0 6px 6px 0;font-family:monospace;font-size:18px;color:#93c5fd;margin:8px 0;line-height:2}
.flag{padding:12px 16px;border-radius:6px;margin-bottom:8px}
table{width:100%;border-collapse:collapse;font-size:18px}
th{background:#080d18;color:#94a3b8;padding:8px 12px;text-align:left;border-bottom:1px solid #1e293b;font-size:16px;text-transform:uppercase;letter-spacing:1px}
td{padding:7px 12px;border-bottom:1px solid #080d18}
.bar-bg{background:#1e293b;border-radius:6px;height:10px;margin-top:4px}
.bar-fill{height:10px;border-radius:6px}
footer{color:#64748b;font-size:16px;text-align:center;margin-top:30px;font-family:monospace}
</style></head><body>)";

    html += QString("<div style='background:#0d1530;border-radius:10px;padding:20px 24px;margin-bottom:20px'>"
                    "<h1>\u269b MUF Calculation Report</h1>"
                    "<div class='sub'>IAEA Safeguards Educational Tool &bull; Generated: %1</div>"
                    "</div>")
                .arg(r.calculatedAt.toString("yyyy-MM-dd HH:mm:ss"));

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
        "<div style='color:#94a3b8;font-size:18px;margin-bottom:6px'>MATERIAL UNACCOUNTED FOR &bull; MUF = IB &minus; EI</div>"
        "<div style='font-size:54px;font-weight:900;color:%2;font-family:monospace'>%3%4 kg</div>"
        "<div style='display:inline-block;margin-top:12px;padding:6px 20px;background:%2;color:#000;font-weight:900;border-radius:20px;font-size:18px'>%5</div>"
        "</div></div>")
        .arg(ratio <= 1 ? "#052e16" : ratio <= 2 ? "#2d1a00" : ratio <= 3 ? "#2d0e00" : "#2d0000")
        .arg(statusCol)
        .arg(r.muf >= 0 ? "+" : "")
        .arg(f6(r.muf))
        .arg(statusTxt);

    // Stat grid
    html += "<div class='card'><h2>Key Metrics</h2><div class='grid'>";
    auto stat = [&](const QString& lbl, const QString& val, const QString& col) {
        return QString("<div class='stat'><div class='sv' style='color:%3'>%2</div><div class='sl'>%1</div></div>")
                   .arg(lbl, val, col);
    };
    html += stat("\u03c3(MUF)",          "\u00b1" + f6(r.sigmaMuf) + " kg", "#c084fc");
    html += stat("|MUF|/\u03c3 RATIO",   QString::number(ratio, 'f', 3) + "\u03c3", statusCol);
    html += stat("BOOK INVENTORY (IB)",  f4(r.bookInventory) + " kg", "#38bdf8");
    html += stat("\u03a3 Inputs",        f4(r.sumInputs)     + " kg", "#34d399");
    html += stat("\u03a3 Outputs",       f4(r.sumOutputs)    + " kg", "#f87171");
    html += stat("ENDING INVENTORY (EI)",f4(in.endingInventory.weight) + " kg", "#fbbf24");
    html += "</div></div>";

    // Equations
    html += "<div class='card'><h2>IAEA Equations \u2014 Step by Step</h2>";
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 1 \u2014 BOOK INVENTORY</p>"
                    "<div class='eq'>IB = BI + &Sigma;Inputs &minus; &Sigma;Outputs<br/>"
                    "IB = %1 + %2 &minus; %3<br/>"
                    "<span style='color:#4ade80;font-weight:700'>IB = %4 kg</span></div>")
                .arg(f4(in.beginningInventory.weight), f4(r.sumInputs), f4(r.sumOutputs), f6(r.bookInventory));
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 2 \u2014 MUF</p>"
                    "<div class='eq'>MUF = IB &minus; EI<br/>"
                    "MUF = %1 &minus; %2<br/>"
                    "<span style='color:#4ade80;font-weight:700'>MUF = %3 kg</span></div>")
                .arg(f6(r.bookInventory), f6(in.endingInventory.weight), f6(r.muf));
    html += QString("<p style='color:#a78bfa;font-weight:700;font-size:17px;letter-spacing:1px'>STEP 3 \u2014 UNCERTAINTY PROPAGATION</p>"
                    "<div class='eq'>&sigma;(MUF) = &radic;[&sigma;&sup2;(BI) + &sigma;&sup2;(&Sigma;In) + &sigma;&sup2;(&Sigma;Out) + &sigma;&sup2;(EI)]<br/>"
                    "&sigma;(MUF) = &radic;[%1 + %2 + %3 + %4]<br/>"
                    "<span style='color:#4ade80;font-weight:700'>&sigma;(MUF) = %5 kg</span></div>")
                .arg(f6(r.sigmaBI * r.sigmaBI),
                     f6(r.sigmaInputs * r.sigmaInputs),
                     f6(r.sigmaOutputs * r.sigmaOutputs),
                     f6(in.endingInventory.uncertainty * in.endingInventory.uncertainty),
                     f6(r.sigmaMuf));
    html += "</div>";

    // Uncertainty table
    html += "<div class='card'><h2>Uncertainty Contributions C&#7522; = [&sigma;&sup2;(i)/&sigma;&sup2;(MUF)] &times; 100%</h2>"
            "<table><tr><th>Term</th><th>\u03c3(i) kg</th><th>\u03c3\u00b2(i) kg\u00b2</th><th>C\u1d62 (%)</th><th style='width:200px'>Visual</th></tr>";
    for (const auto& c : r.contributions) {
        const QString col = c.percentContrib > 50 ? "#ef4444"
                          : c.percentContrib > 25 ? "#f59e0b" : "#3b82f6";
        html += QString("<tr><td style='color:#e2e8f0'>%1</td>"
                        "<td style='font-family:monospace;color:#93c5fd'>%2</td>"
                        "<td style='font-family:monospace;color:#93c5fd'>%3</td>"
                        "<td style='font-weight:900;color:%4'>%5%</td>"
                        "<td><div class='bar-bg'><div class='bar-fill' style='width:%6%;background:%4'></div></div></td></tr>")
                    .arg(c.name)
                    .arg(f6(std::sqrt(c.varianceContrib)))
                    .arg(f6(c.varianceContrib))
                    .arg(col)
                    .arg(c.percentContrib, 0, 'f', 2)
                    .arg(qMin(100.0, c.percentContrib), 0, 'f', 0);
    }
    html += "</table></div>";

    // Diagnostics
    html += "<div class='card'><h2>Safeguards Diagnostics</h2>";
    for (const auto& f : r.flags) {
        QString bg, bc;
        switch (f.severity) {
            case Severity::OK:       bg = "#052e16"; bc = "#16a34a"; break;
            case Severity::INFO:     bg = "#0c1a3a"; bc = "#2563eb"; break;
            case Severity::WARNING:  bg = "#2d1a00"; bc = "#d97706"; break;
            case Severity::ALERT:    bg = "#2d0e00"; bc = "#ea580c"; break;
            case Severity::CRITICAL: bg = "#2d0000"; bc = "#dc2626"; break;
        }
        html += QString("<div class='flag' style='background:%1;border-left:3px solid %2'>"
                        "<span style='background:%2;color:#000;font-size:13px;font-weight:900;padding:2px 8px;border-radius:4px'>%3</span>"
                        "&nbsp;<code style='color:%2;font-weight:700'>%4</code>"
                        "<p style='color:#d1d5db;font-size:18px;margin:6px 0 4px'>%5</p>"
                        "<p style='color:#94a3b8;font-size:17px;margin:0'>&#9658; %6</p>"
                        "</div>")
                    .arg(bg, bc, severityLabel(f.severity), f.code, f.message, f.action);
    }
    html += "</div>";

    html += "<footer>MUFCalc v2.0 &bull; IAEA Safeguards Educational Tool &bull; "
            "Samuel Nartey Teye &bull; Tomsk Polytechnic University 2026 &bull; "
            "FOR EDUCATIONAL USE ONLY</footer></body></html>";
    return html;
}

}  // namespace MUFCalc
