#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: UncertaintyTab
//  Per-term uncertainty contribution bars (Cᵢ progress display).
// ─────────────────────────────────────────────────────────────────────────

#include <QWidget>
#include "../core/Types.h"

class UncertaintyTab : public QWidget {
    Q_OBJECT
public:
    UncertaintyTab(const MUFCalc::MufInput& input,
                   const MUFCalc::MufResult& result,
                   QWidget* parent = nullptr);
};
