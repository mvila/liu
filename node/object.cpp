#include "node/object.h"
#include "node/controlflow.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"
#include "node/object/language/nativemethod.h"

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

LIU_END
