#include "node/object/iterable.h"
#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Iterable ===

LIU_DEFINE(Iterable, Object, Object);

Iterable::Iterable(Node *origin) : Object(origin) {}

void Iterable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable, init);

    LIU_ADD_NATIVE_METHOD(Iterable, iterator);

    LIU_ADD_NATIVE_METHOD(Iterable, contains);
    LIU_ADD_NATIVE_METHOD(Iterable, count);
    LIU_ADD_NATIVE_METHOD(Iterable, size);
    LIU_ADD_NATIVE_METHOD(Iterable, empty);

    LIU_ADD_NATIVE_METHOD(Iterable, equal_to, ==);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, init) {
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, iterator) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return iterator();
}

bool Iterable::contains(Node *what) const {
    QScopedPointer<Iterator> i(iterator());
    while(i->hasNext()) if(i->next()->isEqualTo(what)) return true;
    return false;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, contains) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(1);
    Node *what = message->runFirstInput();
    return LIU_BOOLEAN(contains(what));
}

int Iterable::count(Node *what) const {
    QScopedPointer<Iterator> i(iterator());
    int counter = 0;
    while(i->hasNext()) if(i->next()->isEqualTo(what)) counter++;
    return counter;
}

LIU_DEFINE_NATIVE_METHOD(Iterable, count) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *what = message->runFirstInput();
    return LIU_NUMBER(count(what));
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

bool Iterable::isEqualTo(const Node *other) const {
    const Iterable *otherIterable = Iterable::dynamicCast(other);
    return otherIterable && false; // [...]
}

LIU_DEFINE_NATIVE_METHOD(Iterable, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(false); // [...]
}

//QString Iterable::toString(bool debug, short level) const {
//    Q_UNUSED(level);
//    return debug ? "\"[...]\"" : "[...]";
//}

// === Iterable::Iterator ===

LIU_DEFINE(Iterable::Iterator, Object, Iterable);

Iterable::Iterator::Iterator(Node *origin) : Object(origin) {}

void Iterable::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, init);

    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, read);
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, skip);

    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, equal_to, ==);
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, init) {
    return this;
}

Node *Iterable::Iterator::next() {
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

bool Iterable::Iterator::isEqualTo(const Node *other) const {
    const Iterator *otherIterator = Iterator::dynamicCast(other);
    return otherIterator && false; // [...]
}

LIU_DEFINE_NATIVE_METHOD(Iterable::Iterator, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(false); // [...]
}

//QString Iterable::Iterator::toString(bool debug, short level) const {
//    Q_UNUSED(level);
//    return debug ? "\"[...]\"" : "[...]";
//}

LIU_END
