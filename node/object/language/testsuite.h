#ifndef LIU_LANGUAGE_TEST_SUITE_H
#define LIU_LANGUAGE_TEST_SUITE_H

#include "node/object/list.h"
#include "node/object/language/test.h"

LIU_BEGIN

namespace Language {
    class TestSuite : public List {
        LIU_DECLARE_2(TestSuite, List, Language);
    public:
        explicit TestSuite(Node *origin = context()->child("Object", "Language", "TestSuite")) :
            List(origin) {}

        TestSuite *init();

        virtual Node *run(Node *receiver = context());
    };
}

LIU_END

#endif // LIU_LANGUAGE_TEST_SUITE_H
