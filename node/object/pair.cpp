#include "node/object/pair.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Pair, Object, Object);

Pair *Pair::init(Node *first, Node *second) {
    Object::init();
    setFirst(first);
    setSecond(second);
    return this;
}

Pair *Pair::initCopy(const Pair *other) {
    Object::initCopy(other);
    setFirst(other->_first);
    setSecond(other->_second);
    return this;
}

Pair::~Pair() {
    setFirst();
    setSecond();
}

void Pair::initRoot() {
    LIU_ADD_NATIVE_METHOD(Pair, init);

    LIU_ADD_PROPERTY(Pair, first)
    LIU_ADD_PROPERTY(Pair, second)
}

LIU_DEFINE_NATIVE_METHOD(Pair, init) {
    LIU_FIND_LAST_MESSAGE;
    if(message->hasAnInput()) {
        LIU_CHECK_INPUT_SIZE(2);
        setFirst(message->runFirstInput());
        setSecond(message->runSecondInput());
    }
    return this;
}

LIU_DEFINE_NODE_ACCESSOR(Pair, Node, first, First);
LIU_DEFINE_NODE_PROPERTY(Pair, first, First);

LIU_DEFINE_NODE_ACCESSOR(Pair, Node, second, Second);
LIU_DEFINE_NODE_PROPERTY(Pair, second, Second);

LIU_END
