#include "node/object/comparable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Comparable, Object, Object);

void Comparable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Comparable, less_than, <);
    LIU_ADD_NATIVE_METHOD(Comparable, less_than_or_equal_to, <=);
    LIU_ADD_NATIVE_METHOD(Comparable, greater_than, >);
    LIU_ADD_NATIVE_METHOD(Comparable, greater_than_or_equal_to, >=);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, less_than) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() < 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, less_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() <= 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, greater_than) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() > 0);
}

LIU_DEFINE_NATIVE_METHOD(Comparable, greater_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() >= 0);
}

LIU_END
