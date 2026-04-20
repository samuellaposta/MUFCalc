#pragma once
#include "datatypes.h"
class ValidationEngine {
public:
    QVector<ValidationResult> validate(const MBPInput& in) const;
};
