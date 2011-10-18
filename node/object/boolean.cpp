#include "node/object/boolean.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Boolean, Object, Object);

Boolean *Boolean::init(const bool *value) {
    Object::init();
    setValue(value);
    return this;
}

Boolean *Boolean::initCopy(const Boolean *other) {
    Object::initCopy(other);
    setValue(other->_value);
    return this;
}

Boolean::~Boolean() {
    setValue();
}

void Boolean::initRoot() {
    LIU_ADD_NATIVE_METHOD(Boolean, init);

    LIU_ADD_PROPERTY(Boolean, value);
}

LIU_DEFINE_NATIVE_METHOD(Boolean, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasInput(0)) setValue(message->runFirstInput()->toBool());
    return this;
}

LIU_DEFINE_ACCESSOR(Boolean, bool, value, Value, false);

LIU_DEFINE_NATIVE_METHOD(Boolean, value_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(Boolean::cast(parent())->hasValue());
    else
        return parent();
}

LIU_DEFINE_NATIVE_METHOD(Boolean, value_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Boolean::cast(parent())->setValue(message->runFirstInput()->toBool());
    return this;
}

bool Boolean::isEqualTo(const Node *other) const {
    const Boolean *otherBoolean = Boolean::dynamicCast(other);
    return otherBoolean && value() == otherBoolean->value();
}

LIU_END
