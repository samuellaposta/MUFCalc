#pragma once
#include "datatypes.h"

class UncertaintyEngine {
public:
    void compute(const MBPInput& input, MUFResult& result) const;
    static double variance(const NuclearItem& item) { return item.error * item.error; }
    static double combinedVariance(const QVector<NuclearItem>& items);
    static double combinedSigma(const QVector<NuclearItem>& items);
private:
    QVector<UncertaintyContribution> buildContributions(const MBPInput& in, double varMUF) const;
};
