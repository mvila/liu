#ifndef LIU_RUN_ALL_TESTS_H
#define LIU_RUN_ALL_TESTS_H

#include "tests/nodetest.h"
#include "tests/objecttest.h"
#include "tests/texttest.h"
#include "tests/listtest.h"
#include "tests/dictionarytest.h"

LIU_BEGIN

static int testFailureCount;

static void beginTests() {
    testFailureCount = 0;
    P("=================== Running all tests ===================");
}

static void test(QObject *testClass) {
    testFailureCount += QTest::qExec(testClass);
}

static void endTests() {
    if(testFailureCount == 0) {
        P("==================== All tests passed ===================");
    } else {
        P(QString("==================== %1 test(s) failed ===================").arg(testFailureCount));
    }
}

static void runAllTests() {
    try {
        beginTests();
        test(new NodeTest);
        test(new ObjectTest);
        test(new TextTest);
        test(new ListTest);
        test(new DictionaryTest);
        endTests();
    } catch(const Exception &e) {
        qDebug() << e.report().toUtf8();
    }
}

LIU_END

#endif // LIU_RUN_ALL_TESTS_H
