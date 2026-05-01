#include "UncertaintyTab.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>

using namespace MUFCalc;

UncertaintyTab::UncertaintyTab(const MufInput& /*input*/,
                                const MufResult& result,
                                QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame);
    auto* w = new QWidget();
    auto* layout = new QVBoxLayout(w);
    layout->setContentsMargins(20, 20, 20, 20); layout->setSpacing(6);

    auto* hdr = new QLabel(QStringLiteral("Uncertainty Contribution Analysis"));
    hdr->setStyleSheet("color:#1a3a5c;font-size:15px;font-weight:900;margin-bottom:4px;");
    layout->addWidget(hdr);

    auto* sumInfo = new QLabel(QString(
            "\u03c3\u00b2(MUF) = <b style='color:#c084fc'>%1 kg\u00b2</b>"
            "  &nbsp;|&nbsp;  \u03c3(MUF) = <b style='color:#c084fc'>\u00b1%2 kg</b>"
            "  &nbsp;|&nbsp;  Formula: C&#7522; = [\u03c3\u00b2(i) / \u03c3\u00b2(MUF)] \u00d7 100%")
            .arg(result.varianceMuf, 0, 'f', 6)
            .arg(result.sigmaMuf,    0, 'f', 6));
    sumInfo->setTextFormat(Qt::RichText);
    sumInfo->setStyleSheet("color:#111827;font-size:14px;background:#f9fafb;border:1px solid #e5e7eb;"
                           "border-radius:7px;padding:11px 16px;margin-bottom:16px;");
    sumInfo->setWordWrap(true);
    layout->addWidget(sumInfo);

    for (const auto& c : result.contributions) {
        const QString col = c.percentContrib > 50 ? "#ef4444"
                          : c.percentContrib > 25 ? "#f59e0b" : "#3b82f6";

        auto* row = new QWidget();
        auto* rl = new QVBoxLayout(row);
        rl->setContentsMargins(0, 0, 0, 12); rl->setSpacing(5);

        auto* topRow = new QHBoxLayout();
        auto* nameLbl = new QLabel(c.name);
        nameLbl->setStyleSheet("color:#1a1a2e;font-size:14px;");
        auto* pctLbl = new QLabel(QString("%1%").arg(c.percentContrib, 0, 'f', 2));
        pctLbl->setStyleSheet(QString("color:%1;font-weight:900;font-family:'Courier New',monospace;"
                                      "font-size:15px;").arg(col));
        topRow->addWidget(nameLbl); topRow->addStretch(); topRow->addWidget(pctLbl);
        rl->addLayout(topRow);

        auto* bar = new QProgressBar();
        bar->setRange(0, 1000);
        bar->setValue(static_cast<int>(std::min(100.0, c.percentContrib) * 10));
        bar->setTextVisible(false);
        bar->setFixedHeight(12);
        bar->setStyleSheet(QString(
            "QProgressBar{background:#e5e7eb;border-radius:6px;border:none;}"
            "QProgressBar::chunk{background:%1;border-radius:6px;}").arg(col));
        rl->addWidget(bar);

        auto* details = new QLabel(QString("\u03c3\u00b2(i) = %1 kg\u00b2   |   \u03c3(i) = %2 kg")
                                       .arg(c.varianceContrib, 0, 'f', 6)
                                       .arg(std::sqrt(c.varianceContrib), 0, 'f', 6));
        details->setStyleSheet("color:#374151;font-size:13px;font-family:'Courier New',monospace;");
        rl->addWidget(details);
        layout->addWidget(row);
    }
    layout->addStretch();
    scroll->setWidget(w);
    root->addWidget(scroll);
}
