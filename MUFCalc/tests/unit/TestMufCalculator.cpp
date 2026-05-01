// ─────────────────────────────────────────────────────────────────────────
//  TestMufCalculator
//  Unit tests for the orchestrator.
// ─────────────────────────────────────────────────────────────────────────

#include <QtTest/QtTest>
#include "../../src/core/MufCalculator.h"

using namespace MUFCalc;

class TestMufCalculator : public QObject {
    Q_OBJECT
private slots:
    void testPerfectBalance();
    void testKnownMuf();
    void testUncertaintyPropagation();
    void testContributionsSumTo100();
    void testNegativeMuf();
    void testInvalidInputRejected();
    void testBiasIsInformational();
};

namespace {
MufInput makeValidInput() {
    MufInput in;
    in.facilityName  = "Test Facility";
    in.facilityId    = "TST-01";
    in.materialType  = "LEU";
    in.operatorName  = "Tester";
    in.mbpId         = "MBP-2026-Q1";
    in.periodStart   = QDateTime::currentDateTime().addDays(-30);
    in.periodEnd     = QDateTime::currentDateTime();
    in.beginningInventory = { "BI", 1000.0, 10.0, 0.0, 0.01 };
    in.inputs  .append({ "Receipts",  500.0, 5.0, 0.0, 0.01 });
    in.outputs .append({ "Shipments", 300.0, 3.0, 0.0, 0.01 });
    in.endingInventory    = { "EI", 1200.0, 12.0, 0.0, 0.01 };
    return in;
}
}

void TestMufCalculator::testPerfectBalance() {
    MufCalculator calc;
    auto r = calc.calculate(makeValidInput());
    QVERIFY(r.success);
    QCOMPARE(r.bookInventory, 1200.0);            // 1000 + 500 - 300
    QVERIFY(qAbs(r.muf) < 1e-9);                  // exactly balanced
}

void TestMufCalculator::testKnownMuf() {
    MufInput in = makeValidInput();
    in.endingInventory.weight = 1195.0;           // 5 kg short
    MufCalculator calc;
    auto r = calc.calculate(in);
    QVERIFY(r.success);
    QCOMPARE(r.muf, 5.0);
    QVERIFY(r.ratio > 0.0);
}

void TestMufCalculator::testUncertaintyPropagation() {
    MufInput in = makeValidInput();
    MufCalculator calc;
    auto r = calc.calculate(in);
    const double expected = std::sqrt(10.0*10.0 + 5.0*5.0 + 3.0*3.0 + 12.0*12.0);
    QVERIFY(qAbs(r.sigmaMuf - expected) < 1e-9);
}

void TestMufCalculator::testContributionsSumTo100() {
    MufCalculator calc;
    auto r = calc.calculate(makeValidInput());
    double total = 0.0;
    for (const auto& c : r.contributions) total += c.percentContrib;
    QVERIFY(qAbs(total - 100.0) < 1e-6);
}

void TestMufCalculator::testNegativeMuf() {
    MufInput in = makeValidInput();
    in.endingInventory.weight = 1210.0;           // 10 kg over: MUF = -10
    MufCalculator calc;
    auto r = calc.calculate(in);
    QCOMPARE(r.muf, -10.0);
}

void TestMufCalculator::testInvalidInputRejected() {
    MufInput in = makeValidInput();
    in.beginningInventory.weight = -5.0;          // illegal
    MufCalculator calc;
    auto r = calc.calculate(in);
    QVERIFY(!r.success);
    QVERIFY(!r.errorMessage.isEmpty());
}

void TestMufCalculator::testBiasIsInformational() {
    MufInput a = makeValidInput();
    MufInput b = makeValidInput();
    b.inputs[0].bias = +0.5;                      // bias is informational only
    MufCalculator c1, c2;
    auto ra = c1.calculate(a);
    auto rb = c2.calculate(b);
    QCOMPARE(ra.muf,      rb.muf);
    QCOMPARE(ra.sigmaMuf, rb.sigmaMuf);           // σ unchanged by bias
}

QTEST_APPLESS_MAIN(TestMufCalculator)
#include "TestMufCalculator.moc"
