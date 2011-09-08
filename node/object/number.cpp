#include "node/object/number.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE(Number, Element, Object);

void Number::initRoot() {
    LIU_ADD_NATIVE_METHOD(Number, init);

    LIU_ADD_NATIVE_METHOD(Number, add, +);
    LIU_ADD_NATIVE_METHOD(Number, subtract, -);
    LIU_ADD_NATIVE_METHOD(Number, multiply, *);
    LIU_ADD_NATIVE_METHOD(Number, divide, /);
    LIU_ADD_NATIVE_METHOD(Number, modulo, %);

    LIU_ADD_NATIVE_METHOD(Number, unary_plus, unary+);
    LIU_ADD_NATIVE_METHOD(Number, unary_minus, unary-);

    LIU_ADD_NATIVE_METHOD(Number, prefix_increment, prefix++);
    LIU_ADD_NATIVE_METHOD(Number, prefix_decrement, prefix--);

    LIU_ADD_NATIVE_METHOD(Number, nan);
    LIU_ADD_NATIVE_METHOD(Number, inf);

    LIU_ADD_NATIVE_METHOD(Number, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Number, compare, <=>);
}

LIU_END
