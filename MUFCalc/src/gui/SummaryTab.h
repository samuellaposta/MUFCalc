#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: SummaryTab
//  Headline MUF banner + stat-card grid + dominant-uncertainty source.
// ─────────────────────────────────────────────────────────────────────────

#include <QWidget>
#include "../core/Types.h"

class SummaryTab : public QWidget {
    Q_OBJECT
public:
    SummaryTab(const MUFCalc::MufInput& input,
               const MUFCalc::MufResult& result,
               QWidget* parent = nullptr);
private:
    const MUFCalc::MufInput&  m_input;
    const MUFCalc::MufResult& m_result;
    QString ratioColorCSS() const;
    QString statusLabel()   const;
};
