#include "node/object/comparable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Comparable, Object, Object);

Comparable *Comparable::init() {
    Object::init();
    return this;
}

Comparable *Comparable::initCopy(const Comparable *other) {
    Object::initCopy(other);
    return this;
}

Comparable::~Comparable() {}

void Comparable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Comparable, compare, <=>);
    LIU_ADD_NATIVE_METHOD(Comparable, less_than, <);
    LIU_ADD_NATIVE_METHOD(Comparable, less_than_or_equal_to, <=);
    LIU_ADD_NATIVE_METHOD(Comparable, greater_than, >);
    LIU_ADD_NATIVE_METHOD(Comparable, greater_than_or_equal_to, >=);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, compare) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Number::make(compare(message->runFirstInput()));
}

LIU_DEFINE_NATIVE_METHOD(Comparable, less_than) {
    LIU_FIND_LAST_MESSAGE;
    return Boolean::make(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() < 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, less_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return Boolean::make(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() <= 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, greater_than) {
    LIU_FIND_LAST_MESSAGE;
    return Boolean::make(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() > 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, greater_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return Boolean::make(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() >= 0);
}

LIU_END
