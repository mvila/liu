#include "node/object/dictionary.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Dictionary ===

LIU_DEFINE_2(Dictionary, List, Object);

Dictionary *Dictionary::init() {
    List::init();
    return this;
}

Dictionary *Dictionary::initCopy(const Dictionary *other) {
    List::initCopy(other);
    if(other->_indexes) _indexes = new QHash<Node::Reference, Node *>(*other->_indexes);
    return this;
}

Dictionary::~Dictionary() {
    delete _indexes;
}

void Dictionary::initRoot() {}

// --- Indexable ---

void Dictionary::append(Node *index, Node *value, bool *okPtr) {
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(okPtr);
//    int max = size();
//    int i = index ? index->toDouble() : max;
//    if(i < 0) i = max + i;
//    bool ok = i == max;
//    if(ok) _insert(i, value);
//    if(okPtr)
//        *okPtr = ok;
//    else if(!ok) {
//        if(i < max)
//            LIU_THROW(DuplicateException, "index already exists");
//        else
//            LIU_THROW(IndexOutOfBoundsException, "index is invalid");
//    }
}

// --- Insertable ---

void Dictionary::insert(Node *index, Node *item, Node *before, bool *okPtr) {
    Q_UNUSED(index);
    Q_UNUSED(item);
    Q_UNUSED(before);
    Q_UNUSED(okPtr);
//    int max = size();
//    int i = index ? index->toDouble() : max;
//    if(i < 0) i = max + i;
//    bool ok = i >= 0 && i <= max;
//    if(ok) _insert(i, item);
//    if(okPtr)
//        *okPtr = ok;
//    else if(!ok) {
//        LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
//    }
}

LIU_END
