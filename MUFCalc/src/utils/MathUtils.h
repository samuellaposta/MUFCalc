#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: MathUtils
//  Small statistical helpers (mean, stddev, two-sided z-test).
// ─────────────────────────────────────────────────────────────────────────

#include <QVector>
#include <cmath>

namespace MUFCalc::MathUtils {

inline double mean(const QVector<double>& xs) {
    if (xs.isEmpty()) return 0.0;
    double s = 0.0;
    for (double x : xs) s += x;
    return s / xs.size();
}

inline double sampleStdDev(const QVector<double>& xs) {
    if (xs.size() < 2) return 0.0;
    const double mu = mean(xs);
    double s = 0.0;
    for (double x : xs) s += (x - mu) * (x - mu);
    return std::sqrt(s / (xs.size() - 1));
}

inline double rss(double a, double b) {                  // root-sum-square
    return std::sqrt(a * a + b * b);
}

}  // namespace MUFCalc::MathUtils
