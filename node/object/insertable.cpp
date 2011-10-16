#include "node/object/insertable.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Insertable, Indexable, Object);

Insertable *Insertable::init() {
    Indexable::init();
    return this;
}

Insertable *Insertable::initCopy(const Insertable *other) {
    Indexable::initCopy(other);
    return this;
}

Insertable::~Insertable() {}

void Insertable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Insertable, insert, []<<);
}

LIU_DEFINE_NATIVE_METHOD(Insertable, insert) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1, 2);
    Node *index = message->hasASecondInput() ? message->runFirstInput() : NULL;
    Node *item = message->runLastInput();
    bool wasFound = true;
    insert(index, item, NULL, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return this;
}

LIU_END
