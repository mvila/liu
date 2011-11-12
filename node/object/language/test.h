#ifndef LIU_LANGUAGE_TEST_H
#define LIU_LANGUAGE_TEST_H

#include "node/object/property.h"
#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    class Test : public Object {
        LIU_DECLARE_2(Test, Object, Language);
    public:
        explicit Test(Node *origin = context()->child("Object", "Language", "Test")) :
            Object(origin), _section(NULL), _receiver(NULL) {}

        static Test *make(Section *section, Node *receiver) { return (new Test())->init(section, receiver); }

        Test *init(Section *section = NULL, Node *receiver = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Section, section, Section);
        LIU_DECLARE_PROPERTY(section);

        LIU_DECLARE_NODE_ACCESSOR(Node, receiver, Receiver);
        LIU_DECLARE_PROPERTY(receiver);

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
        Section *_section;
        Node *_receiver;
    };
}

LIU_END

#endif // LIU_LANGUAGE_TEST_H
