#include "node/object/insertable.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Insertable, Indexable, Object);

void Insertable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Insertable, insert, []<<);
}

LIU_DEFINE_NATIVE_METHOD(Insertable, insert) {
    PP;
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *index = message->runFirstInput();
    bool wasFound = true;
    Node *result = get(index, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}

LIU_END
