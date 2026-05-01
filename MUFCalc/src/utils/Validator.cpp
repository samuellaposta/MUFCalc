#include "Validator.h"
#include "../app/Constants.h"
#include <QRegularExpression>
#include <cmath>

namespace MUFCalc {

namespace {
// Block SQL/script injection. Parentheses are intentionally allowed —
// IAEA labels routinely use them, e.g. "Feed Cylinders (KMP-A)".
bool isSafeText(const QString& s) {
    static const QRegularExpression blocked(R"([<>;"'\\&|`$\{\}\[\]\x00-\x1F])");
    return !s.contains(blocked);
}
bool isAlphaNumericId(const QString& s) {
    static const QRegularExpression ok(R"(^[A-Za-z0-9\-_\./\s]+$)");
    return ok.match(s).hasMatch();
}
}  // namespace

QVector<ValidationResult> Validator::validate(const MufInput& in) const {
    QVector<ValidationResult> out;
    auto err = [&](const QString& f, const QString& m) { out.append({ false, f, m }); };

    // Text fields
    if (in.facilityName.trimmed().isEmpty())
        err("Facility Name", "Facility name is required.");
    else if (in.facilityName.length() > ValidationLimits::MAX_NAME_LEN)
        err("Facility Name", QString("Facility name must be \u2264 %1 characters.")
                                 .arg(ValidationLimits::MAX_NAME_LEN));
    else if (!isSafeText(in.facilityName))
        err("Facility Name", "Facility name contains invalid characters.");

    if (in.facilityId.trimmed().isEmpty())
        err("Facility ID", "Facility ID is required.");
    else if (in.facilityId.length() > ValidationLimits::MAX_ID_LEN)
        err("Facility ID", QString("Facility ID must be \u2264 %1 characters.")
                               .arg(ValidationLimits::MAX_ID_LEN));
    else if (!isAlphaNumericId(in.facilityId))
        err("Facility ID", "Facility ID must contain only letters, digits, hyphens and underscores.");

    if (in.materialType.trimmed().isEmpty())
        err("Material Type", "Material type must be selected.");

    if (in.operatorName.trimmed().isEmpty())
        err("Operator Name", "Operator name is required.");
    else if (in.operatorName.length() > ValidationLimits::MAX_OPERATOR_LEN)
        err("Operator Name", QString("Operator name must be \u2264 %1 characters.")
                                 .arg(ValidationLimits::MAX_OPERATOR_LEN));
    else if (!isSafeText(in.operatorName))
        err("Operator Name", "Operator name contains invalid characters.");

    if (in.mbpId.trimmed().isEmpty())
        err("MBP ID", "MBP identifier is required.");
    else if (in.mbpId.length() > ValidationLimits::MAX_ID_LEN)
        err("MBP ID", QString("MBP ID must be \u2264 %1 characters.")
                          .arg(ValidationLimits::MAX_ID_LEN));
    else if (!isSafeText(in.mbpId))
        err("MBP ID", "MBP ID contains invalid characters.");

    // Period
    if (!in.periodStart.isValid()) err("Period Start", "Period start date is invalid.");
    if (!in.periodEnd.isValid())   err("Period End",   "Period end date is invalid.");
    if (in.periodStart.isValid() && in.periodEnd.isValid()) {
        const int days = in.periodStart.daysTo(in.periodEnd);
        if (days < 1)        err("Period", "Period end must be after period start.");
        if (days > 365 * 5)  err("Period", "Period exceeds 5 years — verify dates.");
        if (in.periodEnd > QDateTime::currentDateTime().addDays(1))
            err("Period", "Period end date is in the future.");
    }

    // Per-item helpers
    auto chkWeight = [&](const QString& ctx, double w) {
        if (!std::isfinite(w))                          err(ctx, "Weight must be a finite number.");
        else if (w < 0)                                  err(ctx, "Weight must be \u2265 0 kg.");
        else if (w > ValidationLimits::MAX_WEIGHT)
            err(ctx, QString("Weight exceeds maximum (%1 kg).")
                         .arg(ValidationLimits::MAX_WEIGHT, 0, 'g', 4));
    };
    auto chkUnc = [&](const QString& ctx, double u, double w) {
        if (!std::isfinite(u))                           err(ctx, "Uncertainty must be a finite number.");
        else if (u <= 0)                                 err(ctx, "Uncertainty must be > 0 kg.");
        else if (u > ValidationLimits::MAX_UNCERTAINTY)
            err(ctx, QString("Uncertainty exceeds maximum (%1 kg).")
                         .arg(ValidationLimits::MAX_UNCERTAINTY, 0, 'g', 4));
        else if (w > 0 && u > w * 0.5)
            err(ctx, "Uncertainty exceeds 50 % of weight — physically unrealistic.");
    };
    auto chkName = [&](const QString& ctx, const QString& n) {
        if (!n.isEmpty()) {
            if (n.length() > ValidationLimits::MAX_NAME_LEN)
                err(ctx, "Name too long (max " + QString::number(ValidationLimits::MAX_NAME_LEN) + " chars).");
            if (!isSafeText(n))
                err(ctx, "Name contains invalid characters.");
        }
    };

    // BI / EI
    chkWeight("BI Weight",      in.beginningInventory.weight);
    chkUnc   ("BI Uncertainty", in.beginningInventory.uncertainty, in.beginningInventory.weight);
    chkName  ("BI Name",        in.beginningInventory.name);
    chkWeight("EI Weight",      in.endingInventory.weight);
    chkUnc   ("EI Uncertainty", in.endingInventory.uncertainty, in.endingInventory.weight);
    chkName  ("EI Name",        in.endingInventory.name);

    // Transfer counts
    if (in.inputs.isEmpty() && in.outputs.isEmpty())
        err("Transfers", "At least one input or output transfer is required.");
    if (in.inputs.size()  > ValidationLimits::MAX_ITEMS)
        err("Inputs",  QString("Too many input rows (max %1).").arg(ValidationLimits::MAX_ITEMS));
    if (in.outputs.size() > ValidationLimits::MAX_ITEMS)
        err("Outputs", QString("Too many output rows (max %1).").arg(ValidationLimits::MAX_ITEMS));

    // Per-item transfer validation
    for (int i = 0; i < in.inputs.size(); ++i) {
        const QString ctx = QString("Input #%1").arg(i + 1);
        chkWeight(ctx + " Weight",      in.inputs[i].weight);
        chkUnc   (ctx + " Uncertainty", in.inputs[i].uncertainty, in.inputs[i].weight);
        chkName  (ctx + " Name",        in.inputs[i].name);
    }
    for (int i = 0; i < in.outputs.size(); ++i) {
        const QString ctx = QString("Output #%1").arg(i + 1);
        chkWeight(ctx + " Weight",      in.outputs[i].weight);
        chkUnc   (ctx + " Uncertainty", in.outputs[i].uncertainty, in.outputs[i].weight);
        chkName  (ctx + " Name",        in.outputs[i].name);
    }

    // Physical consistency (only run if no other failure was raised)
    if (out.isEmpty()) {
        double sumIn = 0.0, sumOut = 0.0;
        for (const auto& it : in.inputs)  sumIn  += it.weight;
        for (const auto& it : in.outputs) sumOut += it.weight;
        const double IB = in.beginningInventory.weight + sumIn - sumOut;
        if (IB < -1e-6)
            err("Book Inventory",
                QString("Computed IB = %1 kg is negative. "
                        "Outputs exceed BI + Inputs — check all values.")
                    .arg(QString::number(IB, 'f', 4)));
    }
    return out;
}

}  // namespace MUFCalc
