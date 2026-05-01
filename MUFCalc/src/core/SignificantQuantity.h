#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: SignificantQuantity helper
//  IAEA Safeguards Glossary 2022 §3.14
// ─────────────────────────────────────────────────────────────────────────

#include <QString>
#include "../app/Constants.h"

namespace MUFCalc {

class SQResolver {
public:
    /// Maps a free-text material type to the IAEA SQ in kg.
    /// Falls back to LEU (75 kg) for unrecognised types.
    static double forMaterial(const QString& materialType);
};

}  // namespace MUFCalc
