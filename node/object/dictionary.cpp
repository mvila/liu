#include "node/object/dictionary.h"
#include "node/object/text.h"
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
    if(other->_items) {
        _items = new QHash<Node::Reference, Node *>(*other->_items);
        foreach(Node *item, *_items) if(item) addUnnamedChild(item);
    }
    if(other->_indexes) _indexes = other->_indexes->copy();
    return this;
}

Dictionary::~Dictionary() {
    if(_items) {
        foreach(Node *item, *_items) if(item) removeUnnamedChild(item);
        delete _items;
    }
    deleteIndexes();
}

void Dictionary::initRoot() {
    addExtension(Insertable::root());

    setIndexes(List::root()->fork());
    indexes()->setIsFlattened(false);

    LIU_ADD_NATIVE_METHOD(Dictionary, make);
}

LIU_DEFINE_NODE_ACCESSOR(Dictionary, List, indexes, Indexes);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Dictionary, indexes);

LIU_DEFINE_NATIVE_METHOD(Dictionary, make) {
    LIU_FIND_LAST_MESSAGE;
    Dictionary *dict = Dictionary::make();
    for(int i = 0; i < message->numInputs(); ++i) {
        Node *key = NULL;
        Primitive *primitive = message->input(i)->label();
        if(!primitive->isDefined()) LIU_THROW(ArgumentException, "missing key in Dictionary initialization");
        if(primitive->hasNext()) LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
        Message *msg = Message::dynamicCast(primitive->value());
        if(msg) {
            if(msg->inputs(false) || msg->outputs(false) || msg->isEscaped() || msg->isParented()
                    || msg->isEllipsed() || msg->hasCodeInput())
                LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
            key = Text::make(msg->name());
        } else
            key = primitive->run();
        dict->append(key, message->runInput(i));
    }
    return dict;
}

Node *Dictionary::unnamedChild(int index) const {
    int i = index;
    if(_items) {
        foreach(Node *item, *_items) if(item) {
            if(i == 0) return item; else i--;
        }
    }
    if(_indexes)
        return _indexes->unnamedChild(i);
    else
        return NULL;
}

// --- Iterable ---

Dictionary::Iterator *Dictionary::iterator() const {
    return Dictionary::Iterator::make(constCast(this));
}

int Dictionary::size() const {
    return indexes()->size();
}

// --- Collection ---

void Dictionary::append(Node *item) {
    _insert(size(), LIU_NODE(), item);
}

Node *Dictionary::remove(Node *item, bool *wasFoundPtr) {
    Node *result = NULL;
    bool wasFound;
    Node *idx = index(item, &wasFound);
    if(wasFound) result = unset(idx);
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "value not found");
    return result;
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
    if(Dictionary *orig = Dictionary::dynamicCast(origin())) {
        Node *result = orig->_get(index);
        if(result) {
            result = result->fork();
            result->setIsVirtual(true);
            constCast(this)->_set(index, result);
            return result;
        }
    }
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
    if(_items) {
        Node *oldItem = _items->value(index);
        if(oldItem) removeUnnamedChild(oldItem);
    } else
        _items = new QHash<Node::Reference, Node *>;
    _items->insert(index, item);
    addUnnamedChild(item);
}

void Dictionary::append(Node *index, Node *item, bool *okPtr) {
    Q_UNUSED(okPtr);
    bool ok = _get(index) == NULL;
    if(ok) _insert(size(), index ? index : LIU_NODE(), item);
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
    if(!_items) LIU_THROW_NULL_POINTER_EXCEPTION("_items is NULL");
    Node *oldItem = _items->value(index);
    if(oldItem) removeUnnamedChild(oldItem);
    Dictionary *orig = Dictionary::dynamicCast(origin());
    if(orig && orig->_get(index))
        _items->insert(index, NULL);
    else {
        _items->remove(index);
    }
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
    addUnnamedChild(item);
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
    Node *theValue = source()->_get(theIndex);
    if(!theIndex) LIU_THROW_NULL_POINTER_EXCEPTION("index is NULL");
    if(!theValue) LIU_THROW_NULL_POINTER_EXCEPTION("value is NULL");
    return NodeQPair(theIndex, theValue);
}

void Dictionary::Iterator::skipNext() {
    if(!hasNext()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    setIndex(index() + 1);
}

LIU_END
