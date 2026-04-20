#pragma once
#include "datatypes.h"
#include <QString>

class ReportGenerator {
public:
    QString generateHTML     (const MBPInput&, const MUFResult&) const;
    QString generatePlainText(const MBPInput&, const MUFResult&) const;
    bool    saveHTML         (const QString& path, const MBPInput&, const MUFResult&) const;
    bool    savePlainText    (const QString& path, const MBPInput&, const MUFResult&) const;
private:
    QString f6(double v) const;
    QString f4(double v) const;
    QString severityStr(Severity s) const;
    QString severityColor(Severity s) const;
};
