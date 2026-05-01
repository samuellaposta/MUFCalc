#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: Validator
//  Field-level input validation and physical-consistency checks.
//  Returns one ValidationResult per failure; empty vector ⇒ valid input.
// ─────────────────────────────────────────────────────────────────────────

#include "../core/Types.h"

namespace MUFCalc {

class Validator {
public:
    QVector<ValidationResult> validate(const MufInput& input) const;
};

}  // namespace MUFCalc
