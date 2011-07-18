#ifndef LIU_LANGUAGE_TEST_H
#define LIU_LANGUAGE_TEST_H

#include "node/object/element.h"
#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    #define LIU_TEST(ARGS...) \
    new Language::Test(context()->child("Object", "Language", "Test"), ##ARGS)

    class Test : public GenericNodeElement<Section> {
        LIU_DECLARE(Test, Element, Language);
    public:
        explicit Test(Node *origin, Section *section = NULL, Node *receiver = NULL) :
            GenericNodeElement<Section>(origin), _receiver(receiver) { setSection(section); }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Test);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Test, LIU_FORK_IF_NOT_NULL(section()), LIU_FORK_IF_NOT_NULL(receiver()));

        // aliases...
        Section *section() const { return value(); }
        void setSection(Section *section) { setValue(section); }

        Node *receiver() const { return _receiver; }
        void setReceiver(Node *receiver) { _receiver = receiver; }

        virtual Node *run(Node *receiver = context()) {
            Q_UNUSED(receiver);
            LIU_PUSH_RUN(this);
            LIU_PUSH_CONTEXT(this->receiver());
            return section()->run();
        }

        virtual QString toString(bool debug = false, short level = 0) const {
            return section() ? section()->toString(debug, level) : "NULL";
        }
    private:
        Node *_receiver;
    };
}

LIU_END

#endif // LIU_LANGUAGE_TEST_H
