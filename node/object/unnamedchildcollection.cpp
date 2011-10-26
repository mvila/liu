#include "node/object/unnamedchildcollection.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(UnnamedChildCollection, Object, Object);

UnnamedChildCollection *UnnamedChildCollection::init(Node *source) {
    Iterable::init();
    setSource(source);
    return this;
}

UnnamedChildCollection *UnnamedChildCollection::initCopy(const UnnamedChildCollection *other) {
    Iterable::initCopy(other);
    setSource(other->_source);
    return this;
}

UnnamedChildCollection::~UnnamedChildCollection() {
    setSource();
}

void UnnamedChildCollection::initRoot() {
    addExtension(Iterable::root());

    LIU_ADD_NATIVE_METHOD(UnnamedChildCollection, init);

    LIU_ADD_READ_ONLY_PROPERTY(UnnamedChildCollection, source)
}

LIU_DEFINE_NATIVE_METHOD(UnnamedChildCollection, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    setSource(message->runFirstInput());
    return this;
}

LIU_DEFINE_NODE_ACCESSOR(UnnamedChildCollection, Node, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(UnnamedChildCollection, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(UnnamedChildCollection, source);

 // --- Iterable ---

UnnamedChildCollection::Iterator *UnnamedChildCollection::iterator() const {
    return UnnamedChildCollection::Iterator::make(source());
}

// === UnnamedChildCollection::Iterator ===

LIU_DEFINE_2(UnnamedChildCollection::Iterator, Iterable::Iterator, UnnamedChildCollection);

UnnamedChildCollection::Iterator *UnnamedChildCollection::Iterator::init(Node *source, int *index) {
    Iterable::Iterator::init();
    setSource(source);
    setIndex(index);
    return this;
}

UnnamedChildCollection::Iterator *UnnamedChildCollection::Iterator::initCopy(const UnnamedChildCollection::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    setIndex(other->_index);
    return this;
}

UnnamedChildCollection::Iterator::~Iterator() {
    setSource();
    setIndex();
}

void UnnamedChildCollection::Iterator::initRoot() {
    setSource(Node::root()->fork());

    LIU_ADD_READ_ONLY_PROPERTY(UnnamedChildCollection::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(UnnamedChildCollection::Iterator, Node, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(UnnamedChildCollection::Iterator, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(UnnamedChildCollection::Iterator, source);

LIU_DEFINE_ACCESSOR(UnnamedChildCollection::Iterator, int, index, Index, 0);

bool UnnamedChildCollection::Iterator::hasNext() const {
    return source()->unnamedChild(index());
}

NodeQPair UnnamedChildCollection::Iterator::peekNext() const {
    Node *result = source()->unnamedChild(index());
    if(!result) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return NodeQPair(NULL, result);
}

void UnnamedChildCollection::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
