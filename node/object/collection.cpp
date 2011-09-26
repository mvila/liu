#include "node/object/collection.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Collection, Iterable, Object);

void Collection::initRoot() {
    LIU_ADD_NATIVE_METHOD(Collection, append, <<);
    LIU_ADD_NATIVE_METHOD(Collection, remove, []>>);

    LIU_ADD_NATIVE_METHOD(Collection, clear);
}

LIU_DEFINE_NATIVE_METHOD(Collection, append) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Message *msg = Message::dynamicCast(message->firstInput()->value()->value());
    if(msg && msg->name() == "[]") {
        PP;
        msg = msg->fork();
        msg->setName("[]<<");
        return msg->run(this);
    }
    Node *value = message->runFirstInput();
    append(value);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Collection, remove) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = NULL;
    if(Primitive *label = message->firstInput()->label()) { // TODO: DRY!
        Message *msg = Message::dynamicCast(label->value());
        if(msg && msg->name() == "value")
            value = message->runFirstInput();
    }
    if(!value) LIU_THROW(ArgumentException, "a value is exepected (ex.: collection[:123] >>)");
    bool wasFound = true;
    Node *result = remove(value, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    return result;
}

void Collection::clear() {
    bool wasFound;
    while(true) {
        Node *item = first(&wasFound);
        if(wasFound)
            remove(item);
        else
            return;
    }
}

LIU_DEFINE_NATIVE_METHOD(Collection, clear) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    LIU_CHECK_EXCLAMATION_MARK;
    clear();
    return this;
}

LIU_END
