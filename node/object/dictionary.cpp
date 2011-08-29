#include "node/object/dictionary.h"
#include "node/object/message.h"
#include "node/object/text.h"

LIU_BEGIN

LIU_DEFINE(Dictionary, Object, Object);

void Dictionary::initRoot() {
    LIU_ADD_NATIVE_METHOD(Dictionary, init);

    LIU_ADD_NATIVE_METHOD(Dictionary, get, []);

    LIU_ADD_NATIVE_METHOD(Dictionary, size);
    LIU_ADD_NATIVE_METHOD(Dictionary, empty);
}

LIU_DEFINE_NATIVE_METHOD(Dictionary, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 3);
//    for(int i = 0; i < message->numInputs(); ++i) {
//        Node *key = NULL;
//        Primitive *primitive = message->input(i)->label();
//        if(!primitive) LIU_THROW(ArgumentException, "missing key in Dictionary initialization");
//        if(primitive->hasNext()) LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
//        Message *msg = Message::dynamicCast(primitive->value());
//        if(msg) {
//            if(msg->inputs(false) || msg->outputs(false) || msg->isEscaped() || msg->isParented()
//                    || msg->isEllipsed() || msg->hasCodeInput())
//                LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
//            key = LIU_TEXT(msg->name());
//        } else
//            key = primitive->run();
//        set(key, message->runInput(i));
//    }
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Dictionary, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return get(message->runFirstInput());
}

LIU_DEFINE_NATIVE_METHOD(Dictionary, size) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_NUMBER(size());
}

LIU_DEFINE_NATIVE_METHOD(Dictionary, empty) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(isEmpty());
}

LIU_END
