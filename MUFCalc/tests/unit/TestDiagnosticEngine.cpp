// ─────────────────────────────────────────────────────────────────────────
//  TestDiagnosticEngine
//  Verifies the 12 safeguards flag triggers fire under expected conditions.
// ─────────────────────────────────────────────────────────────────────────

#include <QtTest/QtTest>
#include "../../src/core/MufCalculator.h"

using namespace MUFCalc;

class TestDiagnosticEngine : public QObject {
    Q_OBJECT
private slots:
    void testNormalBalanceIsOk();
    void testCriticalMufRaisesFlag();
    void testNegativeMufFlagged();
    void testSqExceededFlagged();
};

namespace {
MufInput baseInput(const QString& mat = "LEU") {
    MufInput in;
    in.facilityName = "Test"; in.facilityId = "TST";
    in.materialType = mat;    in.operatorName = "X"; in.mbpId = "M1";
    in.periodStart = QDateTime::currentDateTime().addDays(-30);
    in.periodEnd   = QDateTime::currentDateTime();
    in.beginningInventory = { "BI", 1000.0, 10.0, 0.0, 0.01 };
    in.inputs  .append({ "In",  500.0, 5.0, 0.0, 0.01 });
    in.outputs .append({ "Out", 300.0, 3.0, 0.0, 0.01 });
    in.endingInventory    = { "EI", 1200.0, 12.0, 0.0, 0.01 };
    return in;
}
bool hasFlag(const MufResult& r, const QString& code) {
    for (const auto& f : r.flags)
        if (f.code == code) return true;
    return false;
}
}

void TestDiagnosticEngine::testNormalBalanceIsOk() {
    MufCalculator calc;
    auto r = calc.calculate(baseInput());
    QVERIFY(hasFlag(r, "MUF-NORMAL"));
}

void TestDiagnosticEngine::testCriticalMufRaisesFlag() {
    MufInput in = baseInput();
    in.endingInventory.weight = 1100.0;           // 100 kg short, way > 3σ
    MufCalculator calc;
    auto r = calc.calculate(in);
    QVERIFY(r.ratio > 3.0);
    QVERIFY(hasFlag(r, "MUF-EXCEED-3\u03c3"));
}

void TestDiagnosticEngine::testNegativeMufFlagged() {
    MufInput in = baseInput();
    in.endingInventory.weight = 1300.0;           // 100 kg over
    MufCalculator calc;
    auto r = calc.calculate(in);
    QVERIFY(r.muf < 0);
    QVERIFY(hasFlag(r, "NEGATIVE-MUF"));
}

void TestDiagnosticEngine::testSqExceededFlagged() {
    MufInput in = baseInput("Pu");                 // SQ = 8 kg
    in.endingInventory.weight = 1190.0;            // MUF = +10 kg > 1 SQ
    MufCalculator calc;
    auto r = calc.calculate(in);
    QVERIFY(r.sqExceeded);
    QVERIFY(hasFlag(r, "SQ-EXCEEDED"));
}

QTEST_APPLESS_MAIN(TestDiagnosticEngine)
#include "TestDiagnosticEngine.moc"
