#include "unnamedchildcollection.h"
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

//UnnamedChildCollection::Iterator *UnnamedChildCollection::iterator() const {
//    return UnnamedChildCollection::Iterator::make(source());
//}

// === UnnamedChildCollection::Iterator ===

//LIU_DEFINE_2(UnnamedChildCollection::Iterator, Iterable::Iterator, UnnamedChildCollection);

//UnnamedChildCollection::Iterator *UnnamedChildCollection::Iterator::init(Node *source) {
//    Iterable::Iterator::init();
//    setSource(source);
//    return this;
//}

//UnnamedChildCollection::Iterator *UnnamedChildCollection::Iterator::initCopy(const UnnamedChildCollection::Iterator *other) {
//    Iterable::Iterator::initCopy(other);
//    setSource(other->_source);
//    return this;
//}

//UnnamedChildCollection::Iterator::~Iterator() {
//    setSource();
//    unsetSourceIterator();
//}

//void UnnamedChildCollection::Iterator::initRoot() {
//    setSource(Node::root()->fork());

//    LIU_ADD_READ_ONLY_PROPERTY(UnnamedChildCollection::Iterator, source)
//}

//LIU_DEFINE_NODE_ACCESSOR(UnnamedChildCollection::Iterator, Node, source, Source);

//void UnnamedChildCollection::Iterator::sourceWillChange() {}
//void UnnamedChildCollection::Iterator::sourceHasChanged() { unsetSourceIterator(); }

//LIU_DEFINE_READ_ONLY_NODE_PROPERTY(UnnamedChildCollection::Iterator, source);

//UnnamedChildCollection::Iterator::SourceIterator *UnnamedChildCollection::Iterator::sourceIterator() const {
//    if(!_sourceIterator) {
//        if(!source()) LIU_THROW_NULL_POINTER_EXCEPTION("source is NULL");
//        constCast(this)->_sourceIterator = new SourceIterator(*source()->_children);
//    }
//    return _sourceIterator;
//}

//void UnnamedChildCollection::Iterator::unsetSourceIterator() {
//    if(_sourceIterator) {
//        delete _sourceIterator;
//        _sourceIterator = NULL;
//    }
//}

//bool UnnamedChildCollection::Iterator::hasNext() const {
//    while(true) {
//        if(!sourceIterator()->hasNext()) break;
//        Node *node = sourceIterator()->peekNext().value();
//        if(node && node->isReal()) break;
//        sourceIterator()->next();
//    }
//    return sourceIterator()->hasNext();
//}

//NodeQPair UnnamedChildCollection::Iterator::peekNext() const {
//    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
//    return NodeQPair(Text::make(sourceIterator()->peekNext().key()), sourceIterator()->peekNext().value());
//}

//void UnnamedChildCollection::Iterator::skipNext() {
//    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
//    sourceIterator()->next();
//}

LIU_END
