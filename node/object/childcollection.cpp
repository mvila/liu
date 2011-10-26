#include "node/object/childcollection.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(ChildCollection, Object, Object);

ChildCollection *ChildCollection::init(Node *source) {
    Iterable::init();
    setSource(source);
    return this;
}

ChildCollection *ChildCollection::initCopy(const ChildCollection *other) {
    Iterable::initCopy(other);
    setSource(other->_source);
    return this;
}

ChildCollection::~ChildCollection() {
    setSource();
}

void ChildCollection::initRoot() {
    addExtension(Iterable::root());

    LIU_ADD_NATIVE_METHOD(ChildCollection, init);

    LIU_ADD_READ_ONLY_PROPERTY(ChildCollection, source)
}

LIU_DEFINE_NATIVE_METHOD(ChildCollection, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    setSource(message->runFirstInput());
    return this;
}

LIU_DEFINE_NODE_ACCESSOR(ChildCollection, Node, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(ChildCollection, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(ChildCollection, source);

 // --- Iterable ---

ChildCollection::Iterator *ChildCollection::iterator() const {
    return ChildCollection::Iterator::make(source());
}

// === ChildCollection::Iterator ===

LIU_DEFINE_2(ChildCollection::Iterator, Iterable::Iterator, ChildCollection);

ChildCollection::Iterator *ChildCollection::Iterator::init(Node *source) {
    Iterable::Iterator::init();
    setSource(source);
    return this;
}

ChildCollection::Iterator *ChildCollection::Iterator::initCopy(const ChildCollection::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    setNamedChildIterator(other->_namedChildIterator);
    setUnnamedChildIterator(other->_unnamedChildIterator);
    setPhase(other->_phase);
    return this;
}

ChildCollection::Iterator::~Iterator() {
    setSource();
    deleteNamedChildIterator();
    deleteUnnamedChildIterator();
    setPhase();
}

void ChildCollection::Iterator::initRoot() {
    setSource(Node::root()->fork());
    setNamedChildIterator(NamedChildDictionary::Iterator::root()->fork());
    setUnnamedChildIterator(UnnamedChildCollection::Iterator::root()->fork());

    LIU_ADD_READ_ONLY_PROPERTY(ChildCollection::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(ChildCollection::Iterator, Node, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(ChildCollection::Iterator, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(ChildCollection::Iterator, source);

LIU_DEFINE_NODE_ACCESSOR(ChildCollection::Iterator, NamedChildDictionary::Iterator, namedChildIterator, NamedChildIterator);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(ChildCollection::Iterator, namedChildIterator);

LIU_DEFINE_NODE_ACCESSOR(ChildCollection::Iterator, UnnamedChildCollection::Iterator, unnamedChildIterator, UnnamedChildIterator);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(ChildCollection::Iterator, unnamedChildIterator);

LIU_DEFINE_ACCESSOR(ChildCollection::Iterator, short, phase, Phase, 0);

Iterable::Iterator *ChildCollection::Iterator::currentIterator() const {
    if(phase() == 0) {
        constCast(this)->setNamedChildIterator(NamedChildDictionary::Iterator::make(source()));
        constCast(this)->setPhase(1);
    }
    if(phase() == 1) {
        if(namedChildIterator()->hasNext()) return namedChildIterator();
        constCast(this)->setUnnamedChildIterator(UnnamedChildCollection::Iterator::make(source()));
        constCast(this)->setPhase(2);
    }
    if(phase() == 2) {
        if(unnamedChildIterator()->hasNext()) return unnamedChildIterator();
        constCast(this)->setPhase(3);
    }
    return NULL;
}

bool ChildCollection::Iterator::hasNext() const {
    return currentIterator() != NULL;
}

NodeQPair ChildCollection::Iterator::peekNext() const {
    Iterable::Iterator *iterator = currentIterator();
    if(!iterator) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return iterator->peekNext();
}

void ChildCollection::Iterator::skipNext() {
    Iterable::Iterator *iterator = currentIterator();
    if(!iterator) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    iterator->skipNext();
}

LIU_END
