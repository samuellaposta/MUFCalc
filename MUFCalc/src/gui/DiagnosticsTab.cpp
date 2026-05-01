#include "DiagnosticsTab.h"
#include "../utils/FormatUtils.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

using namespace MUFCalc;

DiagnosticsTab::DiagnosticsTab(const MufInput& /*input*/,
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
    layout->setContentsMargins(20, 20, 20, 20); layout->setSpacing(10);

    auto* hdr = new QLabel(QString("Safeguards Diagnostic Results \u2014 %1 flag(s) generated")
                               .arg(result.flags.size()));
    hdr->setStyleSheet("color:#1a3a5c;font-size:15px;font-weight:900;margin-bottom:8px;");
    layout->addWidget(hdr);

    for (const auto& f : result.flags) {
        QString bg, border;
        switch (f.severity) {
            case Severity::OK:       bg = "#f0fdf4"; border = "#16a34a"; break;
            case Severity::INFO:     bg = "#eff6ff"; border = "#2563eb"; break;
            case Severity::WARNING:  bg = "#fffbeb"; border = "#d97706"; break;
            case Severity::ALERT:    bg = "#fff7ed"; border = "#ea580c"; break;
            case Severity::CRITICAL: bg = "#fef2f2"; border = "#dc2626"; break;
        }
        auto* card = new QFrame();
        card->setStyleSheet(QString("background:%1;border:1px solid %2;border-left:3px solid %2;"
                                    "border-radius:0 8px 8px 0;").arg(bg, border));
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 12, 16, 12); cl->setSpacing(5);

        auto* topRow = new QHBoxLayout();
        auto* badge = new QLabel(FormatUtils::severityLabel(f.severity));
        badge->setStyleSheet(QString("background:%1;color:#fff;font-size:13px;font-weight:900;"
                                     "padding:3px 10px;border-radius:4px;letter-spacing:1px;").arg(border));
        auto* codeLbl = new QLabel(f.code);
        codeLbl->setStyleSheet(QString("color:%1;font-weight:700;font-size:15px;"
                                       "font-family:'Courier New',monospace;").arg(border));
        topRow->addWidget(badge); topRow->addWidget(codeLbl); topRow->addStretch();
        cl->addLayout(topRow);

        auto* msgLbl = new QLabel(f.message);
        msgLbl->setStyleSheet("color:#1a1a2e;font-size:14px;");
        msgLbl->setWordWrap(true);
        cl->addWidget(msgLbl);

        auto* actLbl = new QLabel(QStringLiteral("\u25b6  ") + f.action);
        actLbl->setStyleSheet("color:#374151;font-size:13px;font-style:italic;");
        actLbl->setWordWrap(true);
        cl->addWidget(actLbl);

        if (!f.reference.isEmpty()) {
            auto* refLbl = new QLabel(QStringLiteral("Ref: ") + f.reference);
            refLbl->setStyleSheet("color:#6b7280;font-size:12px;");
            refLbl->setWordWrap(true);
            cl->addWidget(refLbl);
        }
        layout->addWidget(card);
    }
    layout->addStretch();
    scroll->setWidget(w);
    root->addWidget(scroll);
}
