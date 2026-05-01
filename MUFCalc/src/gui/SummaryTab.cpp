#include "SummaryTab.h"
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

using namespace MUFCalc;

namespace {
QWidget* statCard(const QString& val, const QString& lbl, const QString& col) {
    auto* card = new QFrame();
    card->setStyleSheet("background:white;border:1px solid #e5e7eb;border-radius:10px;padding:4px;");
    auto* cl = new QVBoxLayout(card);
    cl->setContentsMargins(14, 12, 14, 12); cl->setSpacing(4);
    auto* vl = new QLabel(val);
    vl->setStyleSheet(QString("color:%1;font-size:18px;font-weight:900;"
                              "font-family:'Courier New',monospace;").arg(col));
    auto* ll = new QLabel(lbl);
    ll->setStyleSheet("color:#111827;font-size:13px;letter-spacing:1px;font-weight:600;");
    cl->addWidget(vl); cl->addWidget(ll);
    return card;
}
}  // namespace

SummaryTab::SummaryTab(const MufInput& input, const MufResult& result, QWidget* parent)
    : QWidget(parent), m_input(input), m_result(result)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame);
    auto* w = new QWidget();
    auto* layout = new QVBoxLayout(w);
    layout->setContentsMargins(20, 20, 20, 20); layout->setSpacing(16);

    // MUF banner
    const QString col = ratioColorCSS();
    auto* banner = new QFrame();
    banner->setStyleSheet(QString("background:%1;border:1px solid %2;border-radius:12px;")
        .arg(m_result.ratio <= 1 ? "#052e16"
            : m_result.ratio <= 2 ? "#2d1a00"
            : m_result.ratio <= 3 ? "#2d0e00" : "#2d0000")
        .arg(col + "40"));
    auto* bl = new QVBoxLayout(banner);
    bl->setContentsMargins(24, 22, 24, 22); bl->setSpacing(6);
    auto* bsub = new QLabel(QStringLiteral("MATERIAL UNACCOUNTED FOR  \u00b7  MUF = IB \u2212 EI"));
    bsub->setAlignment(Qt::AlignCenter);
    bsub->setStyleSheet("color:#94a3b8;font-size:14px;letter-spacing:1px;");
    auto* bval = new QLabel(QString("%1%2 kg").arg(m_result.muf >= 0 ? "+" : "")
                                              .arg(m_result.muf, 0, 'f', 6));
    bval->setAlignment(Qt::AlignCenter);
    bval->setStyleSheet(QString("color:%1;font-size:34px;font-weight:900;"
                                "font-family:'Courier New',monospace;").arg(col));
    auto* badge = new QLabel(statusLabel());
    badge->setAlignment(Qt::AlignCenter);
    badge->setStyleSheet(QString("background:%1;color:#000;font-weight:900;font-size:15px;"
                                 "padding:7px 22px;border-radius:20px;letter-spacing:1px;").arg(col));
    auto* btime = new QLabel(m_result.calculatedAt.toString("yyyy-MM-dd HH:mm:ss") +
                             "  \u00b7  " + m_input.facilityName + "  \u00b7  " + m_input.mbpId);
    btime->setAlignment(Qt::AlignCenter);
    btime->setStyleSheet("color:#94a3b8;font-size:13px;");
    bl->addWidget(bsub); bl->addWidget(bval); bl->addWidget(badge, 0, Qt::AlignCenter); bl->addWidget(btime);
    layout->addWidget(banner);

    // Stat grid
    auto* grid = new QGridLayout();
    grid->setSpacing(10);
    auto f4 = [](double v) { return QString::number(v, 'f', 4) + " kg"; };
    auto f6 = [](double v) { return QString::number(v, 'f', 6); };
    grid->addWidget(statCard("\u00b1" + f6(m_result.sigmaMuf) + " kg",        "\u03c3(MUF)",            "#c084fc"), 0, 0);
    grid->addWidget(statCard(QString::number(m_result.ratio, 'f', 3) + "\u03c3", "|MUF|/\u03c3 RATIO",  col),       0, 1);
    grid->addWidget(statCard(f4(m_result.bookInventory),                       "BOOK INVENTORY (IB)",  "#38bdf8"), 0, 2);
    grid->addWidget(statCard(f4(m_result.sumInputs),                           "\u03a3 Inputs",         "#34d399"), 1, 0);
    grid->addWidget(statCard(f4(m_result.sumOutputs),                          "\u03a3 Outputs",        "#f87171"), 1, 1);
    grid->addWidget(statCard(f4(m_input.endingInventory.weight),               "ENDING INVENTORY (EI)","#fbbf24"), 1, 2);
    layout->addLayout(grid);

    // Dominant source
    if (!m_result.contributions.isEmpty()) {
        const auto& dom = m_result.contributions.first();
        auto* dc = new QFrame();
        dc->setStyleSheet("background:white;border:1px solid #e5e7eb;border-radius:8px;padding:14px 18px;");
        auto* dl = new QVBoxLayout(dc);
        auto* dt = new QLabel(QStringLiteral("DOMINANT UNCERTAINTY SOURCE"));
        dt->setStyleSheet("color:#111827;font-size:13px;font-weight:700;letter-spacing:2px;");
        auto* dn = new QLabel(dom.name);
        dn->setStyleSheet("color:#c084fc;font-weight:700;font-size:15px;");
        auto* dp = new QLabel(QString("Contributes %1 %% to total \u03c3\u00b2(MUF)")
                                  .arg(dom.percentContrib, 0, 'f', 1));
        dp->setStyleSheet("color:#111827;font-size:14px;");
        dl->addWidget(dt); dl->addWidget(dn); dl->addWidget(dp);
        layout->addWidget(dc);
    }
    layout->addStretch();
    scroll->setWidget(w);
    root->addWidget(scroll);
}

QString SummaryTab::ratioColorCSS() const {
    const double r = m_result.ratio;
    if (r <= 1) return "#22c55e";
    if (r <= 2) return "#f59e0b";
    if (r <= 3) return "#f97316";
    return "#ef4444";
}

QString SummaryTab::statusLabel() const {
    const double r = m_result.ratio;
    if (r <= 1) return QStringLiteral("\u2714  NORMAL  (\u2264 1\u03c3)");
    if (r <= 2) return QStringLiteral("\u26a0  ELEVATED  (1\u03c3 \u2013 2\u03c3)");
    if (r <= 3) return QStringLiteral("\u26a0  WARNING  (2\u03c3 \u2013 3\u03c3)");
    return QStringLiteral("\u2716  CRITICAL  (> 3\u03c3)");
}
