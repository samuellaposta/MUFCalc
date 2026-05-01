#include "ReportGenerator.h"
#include <QFile>
#include <QTextStream>

namespace MUFCalc {

QString ReportGenerator::generate(ReportFormat fmt,
                                  const MufInput& in,
                                  const MufResult& r) const {
    switch (fmt) {
        case ReportFormat::Html:      return m_html.render(in, r);
        case ReportFormat::PlainText: return m_txt.render(in, r);
    }
    return QString();
}

static bool writeText(const QString& path, const QString& contents) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << contents;
    return true;
}

bool ReportGenerator::save(ReportFormat fmt, const QString& path,
                           const MufInput& in, const MufResult& r) const {
    return writeText(path, generate(fmt, in, r));
}

bool ReportGenerator::saveHtml(const QString& p, const MufInput& in, const MufResult& r) const {
    return writeText(p, m_html.render(in, r));
}
bool ReportGenerator::savePlainText(const QString& p, const MufInput& in, const MufResult& r) const {
    return writeText(p, m_txt.render(in, r));
}

}  // namespace MUFCalc
