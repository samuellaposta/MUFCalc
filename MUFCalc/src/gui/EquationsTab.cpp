#include "EquationsTab.h"
#include <QFrame>
#include <QScrollArea>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <cmath>

using namespace MUFCalc;

EquationsTab::EquationsTab(const MufInput& input,
                            const MufResult& result,
                            QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    auto* browser = new QTextBrowser();
    browser->setStyleSheet("background:white;border:none;font-family:'Courier New',monospace;font-size:15px;padding:14px;");
    browser->setOpenExternalLinks(false);

    auto f4 = [](double v) { return QString::number(v, 'f', 4); };
    auto f6 = [](double v) { return QString::number(v, 'f', 6); };

    auto eq = [](const QString& title, const QString& formula,
                 const QString& sub, const QString& res) {
        return QString("<p style='color:#a78bfa;font-weight:700;font-size:14px;letter-spacing:2px;margin:16px 0 4px'>%1</p>"
                       "<div style='background:#f4f5f7;border-left:3px solid #1a3a5c;padding:12px 16px;"
                       "border-radius:0 6px 6px 0;font-family:Courier New,monospace;color:#1a3a5c;line-height:2'>"
                       "%2<br/>%3<br/><span style='color:#15803d;font-weight:700'>= %4</span></div>")
                   .arg(title, formula, sub, res);
    };

    QString html = "<div style='padding:16px;font-family:Arial,sans-serif;'>"
                   "<h3 style='color:#1a3a5c;margin-top:0;font-family:Georgia,serif;'>"
                   "IAEA Standard Equations \u2014 Step-by-Step</h3>";

    html += eq("STEP 1 \u2014 BOOK INVENTORY",
               "IB = BI + &Sigma;Inputs &minus; &Sigma;Outputs",
               QString("IB = %1 + %2 &minus; %3")
                   .arg(f4(input.beginningInventory.weight),
                        f4(result.sumInputs), f4(result.sumOutputs)),
               f6(result.bookInventory) + " kg");

    html += eq("STEP 2 \u2014 MATERIAL UNACCOUNTED FOR (MUF)",
               "MUF = IB &minus; EI",
               QString("MUF = %1 &minus; %2")
                   .arg(f6(result.bookInventory), f6(input.endingInventory.weight)),
               f6(result.muf) + " kg");

    html += eq("STEP 3 \u2014 UNCERTAINTY PROPAGATION",
               "&sigma;(MUF) = &radic;[ &sigma;&sup2;(BI) + &sigma;&sup2;(&Sigma;In) + &sigma;&sup2;(&Sigma;Out) + &sigma;&sup2;(EI) ]",
               QString("&sigma;(MUF) = &radic;[ %1 + %2 + %3 + %4 ]")
                   .arg(f6(result.sigmaBI * result.sigmaBI),
                        f6(result.sigmaInputs * result.sigmaInputs),
                        f6(result.sigmaOutputs * result.sigmaOutputs),
                        f6(input.endingInventory.uncertainty * input.endingInventory.uncertainty)),
               f6(result.sigmaMuf) + " kg");

    html += "<p style='color:#a78bfa;font-weight:700;font-size:14px;letter-spacing:2px;margin:16px 0 4px'>"
            "STEP 4 \u2014 EXPLAINABILITY LAYER &nbsp; "
            "C&#7522; = [&sigma;&sup2;(i) / &sigma;&sup2;(MUF)] &times; 100%</p>";

    html += "<table style='width:100%;border-collapse:collapse;font-size:15px;font-family:Courier New,monospace;'>"
            "<tr style='border-bottom:1px solid #e5e7eb;'>"
            "<th style='color:#111827;padding:8px 12px;text-align:left;'>Term</th>"
            "<th style='color:#111827;padding:8px 12px;'>\u03c3(i) kg</th>"
            "<th style='color:#111827;padding:8px 12px;'>\u03c3\u00b2(i) kg\u00b2</th>"
            "<th style='color:#111827;padding:8px 12px;'>C&#7522; %</th></tr>";
    bool alt = false;
    for (const auto& c : result.contributions) {
        const QString col = c.percentContrib > 50 ? "#ef4444"
                          : c.percentContrib > 25 ? "#f59e0b" : "#15803d";
        html += QString("<tr style='background:%5;border-bottom:1px solid #f3f4f6;'>"
                        "<td style='color:#111827;padding:7px 12px;'>%1</td>"
                        "<td style='color:#1a3a5c;padding:7px 12px;text-align:right;'>%2</td>"
                        "<td style='color:#1a3a5c;padding:7px 12px;text-align:right;'>%3</td>"
                        "<td style='color:%4;padding:7px 12px;text-align:right;font-weight:700;'>%6%</td></tr>")
                    .arg(c.name)
                    .arg(f6(std::sqrt(c.varianceContrib)))
                    .arg(f6(c.varianceContrib))
                    .arg(col)
                    .arg(alt ? "#f9fafb" : "#ffffff")
                    .arg(c.percentContrib, 0, 'f', 2);
        alt = !alt;
    }
    html += "</table></div>";
    browser->setHtml(html);
    root->addWidget(browser);
}
