#include "node/object/language/test.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Test, Object, Language);

    Test *Test::init(Section *section, Node *receiver) {
        Object::init();
        setSection(section);
        setReceiver(receiver);
        return this;
    }

    Test *Test::initCopy(const Test *other) {
        Object::initCopy(other);
        setSection(other->_section);
        setReceiver(other->_receiver);
        return this;
    }

    Test::~Test() {
        setSection();
        setReceiver();
    }

    void Test::initRoot() {
        setSection(Section::root());
        setReceiver(Node::root());

        LIU_ADD_PROPERTY(Test, section);
        LIU_ADD_PROPERTY(Test, receiver);
    }

    LIU_DEFINE_NODE_ACCESSOR(Test, Section, section, Section);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Test, section);
    LIU_DEFINE_NODE_PROPERTY(Test, Section, section, Section);

    LIU_DEFINE_NODE_ACCESSOR(Test, Node, receiver, Receiver);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Test, receiver);
    LIU_DEFINE_NODE_PROPERTY(Test, Node, receiver, Receiver);
}

LIU_END
