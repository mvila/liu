#include "node/alias.h"
#include "node/object/language/message.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE(Alias, Node, Node);

void Alias::initRoot() {
    LIU_ADD_NATIVE_METHOD(Alias, init);
}

LIU_DEFINE_NATIVE_METHOD(Alias, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Primitive *primitive = message->firstInput()->value();
    while(primitive) {
        Message *msg = Message::dynamicCast(primitive->value());
        if(!msg) LIU_THROW(ArgumentException, "expected 'Message'");
        names().append(msg->name());
        primitive = primitive->next();
    }
    return this;
}

LIU_END
