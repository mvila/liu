#include "node/object/dictionary.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Dictionary ===

LIU_DEFINE_2(Dictionary, Object, Object);

Dictionary *Dictionary::init() {
    Insertable::init();
    return this;
}

Dictionary *Dictionary::initCopy(const Dictionary *other) {
    Insertable::initCopy(other);
    if(other->_items) _items = new QHash<Node::Reference, Node *>(*other->_items);
    if(other->_indexes) _indexes = other->_indexes->copy();
    return this;
}

Dictionary::~Dictionary() {
    delete _items;
    setIndexes();
}

void Dictionary::initRoot() {
    addExtension(Insertable::root());

    setIndexes(List::root()->fork());
}

LIU_DEFINE_NODE_ACCESSOR(Dictionary, List, indexes, Indexes);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Dictionary, indexes);

// --- Iterable ---

Dictionary::Iterator *Dictionary::iterator() const {
    return Dictionary::Iterator::make(constCast(this));
}

int Dictionary::size() const {
    return indexes()->size();
}

// --- Indexable ---

Node *Dictionary::get(Node *index, bool *wasFoundPtr) {
    Node *result = _get(index);
    bool wasFound = result != NULL;
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "index not found");
    return result;
}

Node *Dictionary::_get(Node *index) {
    if(_items && _items->contains(index)) return _items->value(index);
    if(Dictionary *orig = Dictionary::dynamicCast(origin())) return orig->_get(index);
    return NULL;
}

void Dictionary::set(Node *index, Node *item, bool *wasFoundPtr) {
    bool wasFound = _get(index) != NULL;
    if(wasFound) _set(index, item);
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "index not found");
}

void Dictionary::_set(Node *index, Node *item) {
    if(!_items) _items = new QHash<Node::Reference, Node *>;
    _items->insert(index, item);
}

void Dictionary::append(Node *index, Node *item, bool *okPtr) {
    Q_UNUSED(okPtr);
    bool ok = _get(index) == NULL;
    if(ok) _insert(size(), index, item);
    if(okPtr)
        *okPtr = ok;
    else if(!ok)
        LIU_THROW(DuplicateException, "index already exists");
}

Node *Dictionary::unset(Node *index, bool *wasFoundPtr) {
    Node *result = _get(index);
    bool wasFound = result != NULL;
    if(wasFound) {
        _unset(index);
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound) {
        LIU_THROW(NotFoundException, "index not found");
    }
    return result;
}

void Dictionary::_unset(Node *index) {
    if(!_items) _items = new QHash<Node::Reference, Node *>;
    Dictionary *orig = Dictionary::dynamicCast(origin());
    if(orig->_get(index))
        _items->insert(index, NULL);
    else
        _items->remove(index);
    indexes()->remove(index);
}

// --- Insertable ---

//void Dictionary::insert(Node *index, Node *item, Node *before, bool *okPtr) {
//    Q_UNUSED(index);
//    Q_UNUSED(item);
//    Q_UNUSED(before);
//    Q_UNUSED(okPtr);
////    int max = size();
////    int i = index ? index->toDouble() : max;
////    if(i < 0) i = max + i;
////    bool ok = i >= 0 && i <= max;
////    if(ok) _insert(i, item);
////    if(okPtr)
////        *okPtr = ok;
////    else if(!ok) {
////        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
////    }
//}

void Dictionary::_insert(int position, Node *index, Node *item) {
    if(!_items) _items = new QHash<Node::Reference, Node *>;
    _items->insert(index, item);
    indexes()->_insert(position, index);
}

// === Dictionary::Iterator ===

LIU_DEFINE_2(Dictionary::Iterator, Iterable::Iterator, Dictionary);

Dictionary::Iterator *Dictionary::Iterator::init(Dictionary *source, int *index) {
    Iterable::Iterator::init();
    setSource(source);
    setIndex(index);
    return this;
}

Dictionary::Iterator *Dictionary::Iterator::initCopy(const Dictionary::Iterator *other) {
    Iterable::Iterator::initCopy(other);
    setSource(other->_source);
    setIndex(other->_index);
    return this;
}

Dictionary::Iterator::~Iterator() {
    setSource();
    setIndex();
}

void Dictionary::Iterator::initRoot() {
    LIU_ADD_READ_ONLY_PROPERTY(Dictionary::Iterator, source)
}

LIU_DEFINE_NODE_ACCESSOR(Dictionary::Iterator, Dictionary, source, Source);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Dictionary::Iterator, source);
LIU_DEFINE_READ_ONLY_NODE_PROPERTY(Dictionary::Iterator, source);

LIU_DEFINE_ACCESSOR(Dictionary::Iterator, int, index, Index, 0);

bool Dictionary::Iterator::hasNext() const {
    return index() < source()->size();
}

NodeQPair Dictionary::Iterator::peekNext() const {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    Node *theIndex = source()->indexes()->_get(index());
    return NodeQPair(theIndex, source()->_get(theIndex));
}

void Dictionary::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
