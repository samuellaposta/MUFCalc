#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: FormatUtils
//  Number- and severity-formatting helpers shared by GUI & reports.
// ─────────────────────────────────────────────────────────────────────────

#include <QString>
#include "../core/Types.h"

namespace MUFCalc::FormatUtils {

inline QString f4(double v) { return QString::number(v, 'f', 4); }
inline QString f6(double v) { return QString::number(v, 'f', 6); }

inline QString severityLabel(Severity s) {
    switch (s) {
        case Severity::OK:       return QStringLiteral("OK");
        case Severity::INFO:     return QStringLiteral("INFO");
        case Severity::WARNING:  return QStringLiteral("WARNING");
        case Severity::ALERT:    return QStringLiteral("ALERT");
        case Severity::CRITICAL: return QStringLiteral("CRITICAL");
    }
    return QStringLiteral("UNKNOWN");
}

inline QString severityColor(Severity s) {
    switch (s) {
        case Severity::OK:       return QStringLiteral("#15803d");
        case Severity::INFO:     return QStringLiteral("#0369a1");
        case Severity::WARNING:  return QStringLiteral("#a16207");
        case Severity::ALERT:    return QStringLiteral("#b45309");
        case Severity::CRITICAL: return QStringLiteral("#b91c1c");
    }
    return QStringLiteral("#374151");
}

}  // namespace MUFCalc::FormatUtils
