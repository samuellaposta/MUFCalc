#include "PlainTextReport.h"
#include "../utils/FormatUtils.h"
#include <QTextStream>

namespace MUFCalc {

QString PlainTextReport::render(const MufInput& in, const MufResult& r) const {
    using FormatUtils::f4;
    using FormatUtils::f6;
    using FormatUtils::severityLabel;

    QString txt;
    QTextStream s(&txt);
    s << "================================================================\n"
      << "  MUFCalc v2.0 \u2014 IAEA Safeguards Educational Tool\n"
      << "================================================================\n"
      << "Generated   : " << r.calculatedAt.toString("yyyy-MM-dd HH:mm:ss") << "\n"
      << "Facility    : " << in.facilityName << " [" << in.facilityId << "]\n"
      << "Material    : " << in.materialType << "\n"
      << "MBP ID      : " << in.mbpId << "\n"
      << "Period      : " << in.periodStart.toString("yyyy-MM-dd")
                          << " to " << in.periodEnd.toString("yyyy-MM-dd") << "\n"
      << "Operator    : " << in.operatorName << "\n\n"
      << "----------------------------------------------------------------\n"
      << "  IAEA EQUATIONS\n"
      << "----------------------------------------------------------------\n"
      << "  BI     = " << f4(in.beginningInventory.weight)
                       << " \u00b1 " << f4(in.beginningInventory.uncertainty) << " kg\n"
      << "  \u03a3 In   = " << f4(r.sumInputs)  << " kg\n"
      << "  \u03a3 Out  = " << f4(r.sumOutputs) << " kg\n"
      << "  IB     = BI + \u03a3In \u2212 \u03a3Out = " << f6(r.bookInventory) << " kg\n"
      << "  EI     = " << f4(in.endingInventory.weight)
                       << " \u00b1 " << f4(in.endingInventory.uncertainty) << " kg\n"
      << "  MUF    = IB \u2212 EI = " << f6(r.muf) << " kg\n"
      << "  \u03c3\u00b2(MUF)= " << f6(r.varianceMuf)
                  << " kg\u00b2  (= \u03c3\u00b2(BI)+\u03c3\u00b2(\u03a3In)+\u03c3\u00b2(\u03a3Out)+\u03c3\u00b2(EI))\n"
      << "  \u03c3(MUF) = " << f6(r.sigmaMuf) << " kg\n"
      << "  Ratio  = |MUF|/\u03c3 = " << QString::number(r.ratio, 'f', 3) << " \u03c3\n\n"
      << "----------------------------------------------------------------\n"
      << "  UNCERTAINTY CONTRIBUTIONS\n"
      << "----------------------------------------------------------------\n";
    for (const auto& c : r.contributions) {
        const QString namePadded = c.name.leftJustified(44, ' ');
        s << QString("  %1  var=%2 kg\u00b2   Ci=%3%\n")
                .arg(namePadded)
                .arg(f6(c.varianceContrib))
                .arg(c.percentContrib, 6, 'f', 2);
    }
    s << "\n----------------------------------------------------------------\n"
      << "  DIAGNOSTIC FLAGS\n"
      << "----------------------------------------------------------------\n";
    for (const auto& f : r.flags) {
        s << "[" << severityLabel(f.severity) << "] " << f.code << "\n"
          << "  " << f.message << "\n"
          << "  \u2192 " << f.action << "\n\n";
    }
    s << "================================================================\n";
    return txt;
}

}  // namespace MUFCalc
