#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: HtmlReport
//  Renders a MufResult as a styled standalone HTML document.
// ─────────────────────────────────────────────────────────────────────────

#include <QString>
#include "../core/Types.h"

namespace MUFCalc {

class HtmlReport {
public:
    QString render(const MufInput& input, const MufResult& result) const;
};

}  // namespace MUFCalc
