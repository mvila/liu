#include "node/object.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/message.h"

LIU_BEGIN

LIU_DEFINE(Object, Node, Node);

void Object::initRoot() {
    addExtension(ControlFlow::root());

    LIU_ADD_NATIVE_METHOD(Object, postfix_increment, postfix++);
    LIU_ADD_NATIVE_METHOD(Object, postfix_decrement, postfix--);

    LIU_ADD_NATIVE_METHOD(Object, add_assign, +=);
    LIU_ADD_NATIVE_METHOD(Object, subtract_assign, -=);
    LIU_ADD_NATIVE_METHOD(Object, multiply_assign, *=);
    LIU_ADD_NATIVE_METHOD(Object, divide_assign, /=);
    LIU_ADD_NATIVE_METHOD(Object, modulo_assign, %=);

    LIU_ADD_NATIVE_METHOD(Object, less_than, <);
    LIU_ADD_NATIVE_METHOD(Object, less_than_or_equal_to, <=);
    LIU_ADD_NATIVE_METHOD(Object, greater_than, >);
    LIU_ADD_NATIVE_METHOD(Object, greater_than_or_equal_to, >=);
}

LIU_DEFINE_NATIVE_METHOD(Object, postfix_increment) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    Node *temp = copy();
    LIU_MESSAGE("prefix++")->run(this);
    return temp;
}

LIU_DEFINE_NATIVE_METHOD(Object, postfix_decrement) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    Node *temp = copy();
    LIU_MESSAGE("prefix--")->run(this);
    return temp;
}

LIU_DEFINE_NATIVE_METHOD(Object, add_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *result = LIU_MESSAGE("+", message->secondInput())->run(message->runFirstInput());
    return LIU_MESSAGE("=", message->firstInput(), result)->run();
}

LIU_DEFINE_NATIVE_METHOD(Object, subtract_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *result = LIU_MESSAGE("-", message->secondInput())->run(message->runFirstInput());
    return LIU_MESSAGE("=", message->firstInput(), result)->run();
}

LIU_DEFINE_NATIVE_METHOD(Object, multiply_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *result = LIU_MESSAGE("*", message->secondInput())->run(message->runFirstInput());
    return LIU_MESSAGE("=", message->firstInput(), result)->run();
}

LIU_DEFINE_NATIVE_METHOD(Object, divide_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *result = LIU_MESSAGE("/", message->secondInput())->run(message->runFirstInput());
    return LIU_MESSAGE("=", message->firstInput(), result)->run();
}

LIU_DEFINE_NATIVE_METHOD(Object, modulo_assign) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *result = LIU_MESSAGE("%", message->secondInput())->run(message->runFirstInput());
    return LIU_MESSAGE("=", message->firstInput(), result)->run();
}

LIU_DEFINE_NATIVE_METHOD(Object, less_than) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() < 0);
}

LIU_DEFINE_NATIVE_METHOD(Object, less_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() <= 0);
}

LIU_DEFINE_NATIVE_METHOD(Object, greater_than) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() > 0);
}

LIU_DEFINE_NATIVE_METHOD(Object, greater_than_or_equal_to) {
    LIU_FIND_LAST_MESSAGE;
    return LIU_BOOLEAN(Number::cast(LIU_MESSAGE("<=>", message->inputs(false))->run(this))->value() >= 0);
}

LIU_END
