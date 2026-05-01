#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: DiagnosticsTab
//  Colour-coded safeguards flag cards.
// ─────────────────────────────────────────────────────────────────────────

#include <QWidget>
#include "../core/Types.h"

class DiagnosticsTab : public QWidget {
    Q_OBJECT
public:
    DiagnosticsTab(const MUFCalc::MufInput& input,
                   const MUFCalc::MufResult& result,
                   QWidget* parent = nullptr);
};
