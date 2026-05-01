// ─────────────────────────────────────────────────────────────────────────
//  TestSignificantQuantity
// ─────────────────────────────────────────────────────────────────────────

#include <QtTest/QtTest>
#include "../../src/core/SignificantQuantity.h"

using namespace MUFCalc;

class TestSignificantQuantity : public QObject {
    Q_OBJECT
private slots:
    void testHEU()       { QCOMPARE(SQResolver::forMaterial("HEU"),  SignificantQuantity::HEU);  }
    void testLEU()       { QCOMPARE(SQResolver::forMaterial("LEU"),  SignificantQuantity::LEU);  }
    void testPu()        { QCOMPARE(SQResolver::forMaterial("Pu"),   SignificantQuantity::Pu);   }
    void testMOX()       { QCOMPARE(SQResolver::forMaterial("MOX"),  SignificantQuantity::Pu);   }
    void testNatU()      { QCOMPARE(SQResolver::forMaterial("Natural Uranium"), SignificantQuantity::NatU); }
    void testThorium()   { QCOMPARE(SQResolver::forMaterial("Thorium"), SignificantQuantity::Th); }
    void testUnknown()   { QCOMPARE(SQResolver::forMaterial("UnknownMaterial"), SignificantQuantity::LEU); }
};

QTEST_APPLESS_MAIN(TestSignificantQuantity)
#include "TestSignificantQuantity.moc"
