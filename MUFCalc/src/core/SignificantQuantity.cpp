#include "SignificantQuantity.h"

namespace MUFCalc {

double SQResolver::forMaterial(const QString& materialType) {
    const QString m = materialType.toLower();
    if (m.contains("pu") || m.contains("mox"))           return SignificantQuantity::Pu;
    if (m.contains("u-233"))                              return SignificantQuantity::U233;
    if (m.contains("heu") || m.contains("high enrich"))  return SignificantQuantity::HEU;
    if (m.contains("depleted") || m.contains(" du"))     return SignificantQuantity::DeplU;
    if (m.contains("natural") || m.contains("natu"))     return SignificantQuantity::NatU;
    if (m.contains("th") || m.contains("thorium"))       return SignificantQuantity::Th;
    return SignificantQuantity::LEU;   // default: most common
}

}  // namespace MUFCalc
