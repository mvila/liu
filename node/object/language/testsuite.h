#ifndef LIU_LANGUAGE_TEST_SUITE_H
#define LIU_LANGUAGE_TEST_SUITE_H

#include "node/object/list.h"
#include "node/object/language/test.h"

LIU_BEGIN

namespace Language {
    #define LIU_TEST_SUITE(ARGS...) \
    new Language::TestSuite(context()->child("Object", "Language", "TestSuite"), ##ARGS)

    class TestSuite : public GenericList<Test *> {
        LIU_DECLARE(TestSuite, List, Language);
    public:
        explicit TestSuite(Node *origin) : GenericList<Test *>(origin) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(TestSuite);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(TestSuite);

        virtual Node *run(Node *receiver = context());
    };
}

LIU_END

#endif // LIU_LANGUAGE_TEST_SUITE_H
