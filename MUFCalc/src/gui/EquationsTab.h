#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: EquationsTab
//  Step-by-step IAEA equation rendering with substituted values.
// ─────────────────────────────────────────────────────────────────────────

#include <QWidget>
#include "../core/Types.h"

class EquationsTab : public QWidget {
    Q_OBJECT
public:
    EquationsTab(const MUFCalc::MufInput& input,
                 const MUFCalc::MufResult& result,
                 QWidget* parent = nullptr);
};
