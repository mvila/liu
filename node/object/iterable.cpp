#include "node/object/iterable.h"
#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Iterable ===

LIU_DEFINE_2(Iterable, Object, Object);

Iterable *Iterable::init() {
    Object::init();
    return this;
}

Iterable *Iterable::initCopy(const Iterable *other) {
    Object::initCopy(other);
    return this;
}

Iterable::~Iterable() {}

void Iterable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable, iterator);

    LIU_ADD_NATIVE_METHOD(Iterable, contains);
    LIU_ADD_NATIVE_METHOD(Iterable, count);
    LIU_ADD_NATIVE_METHOD(Iterable, size);
    LIU_ADD_NATIVE_METHOD(Iterable, empty);

    LIU_ADD_NATIVE_METHOD(Iterable, first);
    LIU_ADD_NATIVE_METHOD(Iterable, second);
    LIU_ADD_NATIVE_METHOD(Iterable, third);
    LIU_ADD_NATIVE_METHOD(Iterable, last);
}

QString Iterable::toString(bool debug, short level) const {
    QString str;
    if(debug) str += "[";
    str += join(debug ? ", " : " ", "", "", debug, level);
    if(debug) str += "]";
    return str;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, iterator) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return iterator();
}

bool Iterable::contains(Node *value) const {
    QScopedPointer<Iterator> i(iterator());
    while(i->hasNext()) if(i->next().second->isEqualTo(value)) return true;
    return false;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, contains) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = message->runFirstInput();
    return Boolean::make(contains(value));
}

int Iterable::count(Node *value) const {
    QScopedPointer<Iterator> i(iterator());
    int counter = 0;
    while(i->hasNext()) if(i->next().second->isEqualTo(value)) counter++;
    return counter;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, count) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = message->runFirstInput();
    return Number::make(count(value));
}

int Iterable::size() const {
    QScopedPointer<Iterator> i(iterator());
    int counter = 0;
    while(i->hasNext()) {
        counter++;
        i->skipNext();
    }
    return counter;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, size) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Number::make(size());
}

bool Iterable::empty() const {
    QScopedPointer<Iterator> i(iterator());
    return !i->hasNext();
}

LIU_DEFINE_NATIVE_METHOD(Iterable, empty) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return Boolean::make(empty());
}

NodeQPair Iterable::first(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, first) { // *** YO ***
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = first(message->isQuestioned() ? &wasFound : NULL).second;
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result;  // TODO: return two results, value and index
}

NodeQPair Iterable::second(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    i->next(wasFoundPtr);
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, second) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = second(message->isQuestioned() ? &wasFound : NULL).second;
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result; // TODO: return two results, value and index
}


NodeQPair Iterable::third(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    i->next(wasFoundPtr);
    i->next(wasFoundPtr);
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, third) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = third(message->isQuestioned() ? &wasFound : NULL).second;
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result; // TODO: return two results, value and index
}

NodeQPair Iterable::last(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    NodeQPair result = NodeQPair(NULL, NULL);
    bool wasFound = false;
    while(i->hasNext()) {
        wasFound = true;
        result = i->next();
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, last) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = last(message->isQuestioned() ? &wasFound : NULL).second;
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result; // TODO: return two results, value and index
}

const QString Iterable::join(const QString &separator, const QString &prefix,
                   const QString &suffix, bool debug, short level) const {
    QString str;
    QScopedPointer<Iterator> i(iterator());
    bool isFirst = true;
    while(i->hasNext()) {
        if(!isFirst) str += separator; else isFirst = false;
        str += prefix + i->next().second->toString(debug, level) + suffix;
    }
    return str;
}

// === Iterator ===

LIU_DEFINE_2(Iterable::Iterator, Object, Iterable);

Iterable::Iterator *Iterable::Iterator::init() {
    Object::init();
    return this;
}

Iterable::Iterator *Iterable::Iterator::initCopy(const Iterable::Iterator *other) {
    Object::initCopy(other);
    return this;
}

Iterable::Iterator::~Iterator() {}

void Iterable::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, read);
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, skip);
}

NodeQPair Iterable::Iterator::next(bool *wasFoundPtr) {
    if(wasFoundPtr) {
        *wasFoundPtr = hasNext();
        if(!*wasFoundPtr) return NodeQPair(NULL, NULL);
    }
    NodeQPair result = peekNext();
    skipNext();
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, read) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned()) return Boolean::make(hasNext());
    NodeQPair result = peekNext();
    if(message->isExclaimed()) skipNext();
    return result.second; // TODO: return two results, value and index
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, skip) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    LIU_CHECK_EXCLAMATION_MARK;
    skipNext();
    return this;
}

LIU_END
