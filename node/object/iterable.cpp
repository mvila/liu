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

// === Iterator ===

LIU_DEFINE_2(Iterable::Iterator, Object, Iterable);

void Iterable::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, read);
    LIU_ADD_NATIVE_METHOD(Iterable::Iterator, skip);
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

LIU_END
