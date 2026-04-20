#include "validationengine.h"
#include <cmath>
#include <QRegularExpression>

// Blocks SQL/script injection and shell special chars.
// Parentheses () are intentionally ALLOWED — standard in IAEA labels like
// "Feed Cylinders (KMP-A)". Dangerous shell patterns $(...) and `...` are
// still blocked via $ and ` being in the list.
static bool isSafeText(const QString& s) {
    static QRegularExpression blocked(R"([<>;"'\\&|`$\{\}\[\]\x00-\x1F])");
    return !s.contains(blocked);
}
static bool isAlphaNumericId(const QString& s) {
    // IDs may contain letters, digits, hyphens, underscores, dots
    static QRegularExpression ok(R"(^[A-Za-z0-9\-_\./\s]+$)");
    return ok.match(s).hasMatch();
}

QVector<ValidationResult> ValidationEngine::validate(const MBPInput& in) const {
    QVector<ValidationResult> out;
    auto err = [&](const QString& f, const QString& m){ out.append({false,f,m}); };

    // ── Text fields ────────────────────────────────────────────────────
    if (in.facilityName.trimmed().isEmpty())
        err("Facility Name","Facility name is required.");
    else if (in.facilityName.length() > ValidationLimits::MAX_LABEL)
        err("Facility Name",QString("Facility name must be ≤ %1 characters.").arg(ValidationLimits::MAX_LABEL));
    else if (!isSafeText(in.facilityName))
        err("Facility Name","Facility name contains invalid characters (< > ; \" ' \\ etc.).");

    if (in.facilityId.trimmed().isEmpty())
        err("Facility ID","Facility ID is required.");
    else if (in.facilityId.length() > ValidationLimits::MAX_FACID)
        err("Facility ID",QString("Facility ID must be ≤ %1 characters.").arg(ValidationLimits::MAX_FACID));
    else if (!isAlphaNumericId(in.facilityId))
        err("Facility ID","Facility ID must contain only letters, digits, hyphens and underscores.");

    if (in.materialType.trimmed().isEmpty())
        err("Material Type","Material type must be selected.");

    if (in.operatorName.trimmed().isEmpty())
        err("Operator Name","Operator name is required.");
    else if (in.operatorName.length() > ValidationLimits::MAX_OPID)
        err("Operator Name",QString("Operator name must be ≤ %1 characters.").arg(ValidationLimits::MAX_OPID));
    else if (!isSafeText(in.operatorName))
        err("Operator Name","Operator name contains invalid characters.");

    if (in.mbpId.trimmed().isEmpty())
        err("MBP ID","MBP identifier is required.");
    else if (in.mbpId.length() > ValidationLimits::MAX_FACID)
        err("MBP ID",QString("MBP ID must be ≤ %1 characters.").arg(ValidationLimits::MAX_FACID));
    else if (!isSafeText(in.mbpId))
        err("MBP ID","MBP ID contains invalid characters.");

    // ── Period ─────────────────────────────────────────────────────────
    if (!in.periodStart.isValid()) err("Period Start","Period start date is invalid.");
    if (!in.periodEnd.isValid())   err("Period End",  "Period end date is invalid.");
    if (in.periodStart.isValid() && in.periodEnd.isValid()) {
        int days = in.periodStart.daysTo(in.periodEnd);
        if (days < 1)        err("Period","Period end must be after period start.");
        if (days > 365*5)    err("Period","Period exceeds 5 years — verify dates.");
        if (in.periodEnd > QDateTime::currentDateTime().addDays(1))
            err("Period","Period end date is in the future.");
    }

    // ── Helper lambdas for item checks ────────────────────────────────
    auto chkQty = [&](const QString& ctx, double q) {
        if (!std::isfinite(q))                      err(ctx,"Quantity must be a finite number.");
        else if (q < 0)                             err(ctx,"Quantity must be ≥ 0 kg.");
        else if (q > ValidationLimits::MAX_QTY)     err(ctx,QString("Quantity exceeds maximum (%1 kg).").arg(ValidationLimits::MAX_QTY,0,'g',4));
    };
    auto chkErr = [&](const QString& ctx, double e, double q) {
        if (!std::isfinite(e))                      err(ctx,"Uncertainty must be a finite number.");
        else if (e <= 0)                            err(ctx,"Uncertainty must be > 0 kg.");
        else if (e > ValidationLimits::MAX_ERROR)   err(ctx,QString("Uncertainty exceeds maximum (%1 kg).").arg(ValidationLimits::MAX_ERROR,0,'g',4));
        else if (q > 0 && e > q * 0.5)             err(ctx,"Uncertainty exceeds 50% of quantity — physically unrealistic.");
    };
    auto chkLabel = [&](const QString& ctx, const QString& lbl) {
        if (!lbl.isEmpty()) {
            if (lbl.length() > ValidationLimits::MAX_LABEL) err(ctx,"Label too long (max "+QString::number(ValidationLimits::MAX_LABEL)+" chars).");
            if (!isSafeText(lbl)) err(ctx,"Label contains invalid characters.");
        }
    };

    // ── BI ─────────────────────────────────────────────────────────────
    chkQty ("BI Quantity",    in.beginningInventory.qty);
    chkErr ("BI Uncertainty", in.beginningInventory.error, in.beginningInventory.qty);
    chkLabel("BI Label",      in.beginningInventory.label);

    // ── EI ─────────────────────────────────────────────────────────────
    chkQty ("EI Quantity",    in.endingInventory.qty);
    chkErr ("EI Uncertainty", in.endingInventory.error, in.endingInventory.qty);
    chkLabel("EI Label",      in.endingInventory.label);

    // ── Transfer counts ─────────────────────────────────────────────────
    if (in.inputs.isEmpty() && in.outputs.isEmpty())
        err("Transfers","At least one input or output transfer is required.");
    if (in.inputs.size()  > ValidationLimits::MAX_ITEMS)
        err("Inputs", QString("Too many input rows (max %1).").arg(ValidationLimits::MAX_ITEMS));
    if (in.outputs.size() > ValidationLimits::MAX_ITEMS)
        err("Outputs",QString("Too many output rows (max %1).").arg(ValidationLimits::MAX_ITEMS));

    // ── Per-item transfer validation ────────────────────────────────────
    for (int i = 0; i < in.inputs.size(); ++i) {
        QString ctx = QString("Input #%1").arg(i+1);
        chkQty  (ctx+" Qty",  in.inputs[i].qty);
        chkErr  (ctx+" Err",  in.inputs[i].error, in.inputs[i].qty);
        chkLabel(ctx+" Label",in.inputs[i].label);
    }
    for (int i = 0; i < in.outputs.size(); ++i) {
        QString ctx = QString("Output #%1").arg(i+1);
        chkQty  (ctx+" Qty",  in.outputs[i].qty);
        chkErr  (ctx+" Err",  in.outputs[i].error, in.outputs[i].qty);
        chkLabel(ctx+" Label",in.outputs[i].label);
    }

    // ── Physical consistency (only if no other errors) ─────────────────
    if (out.isEmpty()) {
        double sumIn = 0, sumOut = 0;
        for (auto& it : in.inputs)  sumIn  += it.qty;
        for (auto& it : in.outputs) sumOut += it.qty;
        double IB = in.beginningInventory.qty + sumIn - sumOut;
        if (IB < -1e-6)
            err("Book Inventory",
                QString("Computed IB = %1 kg is negative. "
                        "Outputs exceed BI + Inputs — check all values.")
                    .arg(QString::number(IB,'f',4)));
    }

    return out;
}
