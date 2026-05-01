#include "ContributionAnalyzer.h"
#include "UncertaintyEngine.h"
#include <algorithm>

namespace MUFCalc {

void ContributionAnalyzer::analyze(const MufInput& in, MufResult& r) const {
    r.contributions.clear();
    r.dominantSource.clear();
    if (r.varianceMuf <= 0.0) return;

    auto push = [&](const QString& name, double v) {
        r.contributions.append({ name, v, (v / r.varianceMuf) * 100.0 });
    };

    push(QString("BI: %1").arg(in.beginningInventory.name.isEmpty()
                              ? QStringLiteral("Beginning Inventory")
                              : in.beginningInventory.name),
         UncertaintyEngine::variance(in.beginningInventory));

    for (const auto& it : in.inputs)
        push(QString("Input: %1").arg(it.name.isEmpty()
                                     ? QStringLiteral("Unnamed") : it.name),
             UncertaintyEngine::variance(it));

    for (const auto& it : in.outputs)
        push(QString("Output: %1").arg(it.name.isEmpty()
                                      ? QStringLiteral("Unnamed") : it.name),
             UncertaintyEngine::variance(it));

    push(QString("EI: %1").arg(in.endingInventory.name.isEmpty()
                              ? QStringLiteral("Ending Inventory")
                              : in.endingInventory.name),
         UncertaintyEngine::variance(in.endingInventory));

    std::sort(r.contributions.begin(), r.contributions.end(),
              [](const UncertaintyContribution& a, const UncertaintyContribution& b) {
                  return a.percentContrib > b.percentContrib;
              });

    if (!r.contributions.isEmpty())
        r.dominantSource = r.contributions.first().name;
}

}  // namespace MUFCalc
