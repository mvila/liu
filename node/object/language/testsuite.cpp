#include "node/object/language/testsuite.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(TestSuite, OldList, Language);

    void TestSuite::initRoot() {
    }

    Node *TestSuite::run(Node *receiver) {
        Q_UNUSED(receiver);
        LIU_PUSH_RUN(this);
        Iterator i(this);
        while(Test *test = i.next())
            test->run();
        return this;
    }
}

LIU_END
