#include "node/object/pair.h"
#include "node/object/comparable.h"
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
    addExtension(Comparable::root());

    setFirst(Node::root());
    setSecond(Node::root());

    LIU_ADD_NATIVE_METHOD(Pair, init);

    LIU_ADD_PROPERTY(Pair, first);
    LIU_ADD_PROPERTY(Pair, second);
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
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Pair, first);
LIU_DEFINE_NODE_PROPERTY(Pair, Node, first, First);

LIU_DEFINE_NODE_ACCESSOR(Pair, Node, second, Second);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Pair, second);
LIU_DEFINE_NODE_PROPERTY(Pair, Node, second, Second);

Node *Pair::unnamedChild(int index) const {
    if(_first && _first->isReal()) {
        if(index == 0) return _first;
        index--;
    }
    if(_second && _second->isReal()) {
        if(index == 0) return _second;
        index--;
    }
    return NULL;
}

bool Pair::isEqualTo(const Node *other) const {
    if(isSameAs(other)) return true;
    const Pair *otherPair = Pair::dynamicCast(other);
    return otherPair && first()->isEqualTo(otherPair->first()) && second()->isEqualTo(otherPair->second());
}

short Pair::compare(const Node *other) const {
    const Pair *otherPair = Pair::dynamicCast(other);
    if(!otherPair) LIU_THROW(ArgumentException, "a Pair is expected");
    short result = first()->compare(otherPair->first());
    if(result == 0) result = second()->compare(otherPair->second());
    return result;
}

uint Pair::hash() const {
    LIU_TODO;
}

LIU_END
