#include "node/object/iterable.h"
#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Iterable ===

LIU_DEFINE_2(Iterable, Object, Object);

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
    QScopedPointer<Iterator> i(iterator());
    bool first = true;
    while(i->hasNext()) {
        if(!first) {
            if(debug) str += ", ";
        } else
            first = false;
        str += i->next()->toString(debug, level);
    }
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
    while(i->hasNext()) if(i->next()->isEqualTo(value)) return true;
    return false;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, contains) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = message->runFirstInput();
    return LIU_BOOLEAN(contains(value));
}

int Iterable::count(Node *value) const {
    QScopedPointer<Iterator> i(iterator());
    int counter = 0;
    while(i->hasNext()) if(i->next()->isEqualTo(value)) counter++;
    return counter;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, count) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = message->runFirstInput();
    return LIU_NUMBER(count(value));
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
    return LIU_NUMBER(size());
}

bool Iterable::empty() const {
    QScopedPointer<Iterator> i(iterator());
    return !i->hasNext();
}

LIU_DEFINE_NATIVE_METHOD(Iterable, empty) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(empty());
}

Node *Iterable::first(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, first) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = first(message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}

Node *Iterable::second(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    i->next(wasFoundPtr);
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, second) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = second(message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}


Node *Iterable::third(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    i->next(wasFoundPtr);
    i->next(wasFoundPtr);
    return i->next(wasFoundPtr);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, third) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    bool wasFound = true;
    Node *result = third(message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}

Node *Iterable::last(bool *wasFoundPtr) const {
    QScopedPointer<Iterator> i(iterator());
    Node *result = NULL;
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
    Node *result = last(message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}

// === Iterator ===

LIU_DEFINE_2(Iterable::Iterator, Object, Iterable);

void Iterable::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, read);
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, skip);
}

Node *Iterable::Iterator::next(bool *wasFoundPtr) {
    if(wasFoundPtr) {
        *wasFoundPtr = hasNext();
        if(!*wasFoundPtr) return NULL;
    }
    Node *result = peekNext();
    skipNext();
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, read) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned()) return LIU_BOOLEAN(hasNext());
    if(message->isExclaimed()) return next();
    return peekNext();
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, skip) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    LIU_CHECK_EXCLAMATION_MARK;
    skipNext();
    return this;
}

LIU_END
