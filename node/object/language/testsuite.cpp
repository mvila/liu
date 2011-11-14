#include "node/object/language/testsuite.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(TestSuite, List, Language);

    TestSuite *TestSuite::init() {
        List::init();
        return this;
    }

    TestSuite *TestSuite::initCopy(const TestSuite *other) {
        List::initCopy(other);
        return this;
    }

    TestSuite::~TestSuite() {}

    void TestSuite::initRoot() {}

    Node *TestSuite::run(Node *receiver) {
        Q_UNUSED(receiver);
        LIU_PUSH_RUN(this);
        QScopedPointer<Iterator> i(iterator());
        while(i->hasNext()) i->next().second->run();
        return this;
    }
}

LIU_END
