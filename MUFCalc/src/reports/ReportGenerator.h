#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: ReportGenerator
//  Thin facade that dispatches to a format-specific renderer.
//  HtmlReport  → standalone HTML
//  PlainTextReport → text
// ─────────────────────────────────────────────────────────────────────────

#include <QString>
#include "HtmlReport.h"
#include "PlainTextReport.h"

namespace MUFCalc {

enum class ReportFormat { Html, PlainText };

class ReportGenerator {
public:
    QString generate(ReportFormat fmt,
                     const MufInput& input,
                     const MufResult& result) const;

    bool save(ReportFormat fmt,
              const QString& path,
              const MufInput& input,
              const MufResult& result) const;

    // Convenience pass-throughs (preserve old call sites)
    QString generateHtml      (const MufInput& in, const MufResult& r) const { return m_html.render(in, r); }
    QString generatePlainText (const MufInput& in, const MufResult& r) const { return m_txt.render(in, r); }
    bool    saveHtml          (const QString& p, const MufInput& in, const MufResult& r) const;
    bool    savePlainText     (const QString& p, const MufInput& in, const MufResult& r) const;

private:
    HtmlReport      m_html;
    PlainTextReport m_txt;
};

}  // namespace MUFCalc
