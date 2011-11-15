#include "node/alias.h"
#include "node/object/language/message.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Alias, Node, Node);

Alias *Alias::init(const QStringList *names) {
    Node::init();
    setNames(names);
    return this;
}

Alias *Alias::initCopy(const Alias *other) {
    Node::initCopy(other);
    setNames(other->_names);
    return this;
}

Alias::~Alias() {
    setNames();
}

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
        namesData()->append(msg->name());
        primitive = primitive->hasNext();
    }
    return this;
}

LIU_DEFINE_ACCESSOR(Alias, QStringList, names, Names,);

LIU_END
