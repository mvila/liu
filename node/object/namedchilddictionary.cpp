#include "namedchilddictionary.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(NamedChildDictionary, Indexable, Object);

NamedChildDictionary *NamedChildDictionary::init(Node *source) {
    Indexable::init();
    setSource(source);
    return this;
}

NamedChildDictionary *NamedChildDictionary::initCopy(const NamedChildDictionary *other) {
    Indexable::initCopy(other);
    setSource(other->_source);
    return this;
}

NamedChildDictionary::~NamedChildDictionary() {
    setSource();
}

void NamedChildDictionary::initRoot() {
    LIU_ADD_READ_ONLY_PROPERTY(NamedChildDictionary, source)
}

LIU_DEFINE_NODE_ACCESSOR(NamedChildDictionary, Node, source, Source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(NamedChildDictionary, source);

// --- Indexable ---

Node *NamedChildDictionary::get(Node *index, bool *wasFoundPtr) {
    QString name = index->toString();
    Node *result = hasDirectChild(name);
    if(result->isVirtual()) result = NULL;
    bool wasFound = result;
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "child not found");
    return result;
}

// === NamedChildDictionary::Iterator ===

LIU_DEFINE_2(NamedChildDictionary::Iterator, Iterable::Iterator, NamedChildDictionary);

NamedChildDictionary::Iterator *NamedChildDictionary::Iterator::init(NamedChildDictionary *theSource) {
    Iterable::Iterator::init();
    setSource(theSource);
    if(theSource) _sourceIterator = new SourceIterator(theSource->children()); // TODO: should use _children!
    return this;
}

NamedChildDictionary::Iterator *NamedChildDictionary::Iterator::initCopy(const NamedChildDictionary::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    _sourceIterator = other->_sourceIterator;
    return this;
}

NamedChildDictionary::Iterator::~Iterator() {
    setSource();
    delete _sourceIterator;
}

void NamedChildDictionary::Iterator::initRoot() {
    setSource(NamedChildDictionary::root()->fork());

    LIU_ADD_READ_ONLY_PROPERTY(NamedChildDictionary::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(NamedChildDictionary::Iterator, NamedChildDictionary, source, Source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(NamedChildDictionary::Iterator, source);

bool NamedChildDictionary::Iterator::hasNext() const {
    while(true) {
        if(!_sourceIterator->hasNext()) break;
        Node *node = _sourceIterator->peekNext().value();
        if(node && node->isReal()) break;
        _sourceIterator->next();
    }
    return _sourceIterator->hasNext();
}

Node *NamedChildDictionary::Iterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return _sourceIterator->peekNext().value();
}

void NamedChildDictionary::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    _sourceIterator->next();
}

LIU_END
