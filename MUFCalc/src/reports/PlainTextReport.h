#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: PlainTextReport
// ─────────────────────────────────────────────────────────────────────────

#include <QString>
#include "../core/Types.h"

namespace MUFCalc {

class PlainTextReport {
public:
    QString render(const MufInput& input, const MufResult& result) const;
};

}  // namespace MUFCalc
