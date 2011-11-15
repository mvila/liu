#ifndef LIU_OBJECT_H
#define LIU_OBJECT_H

#include "node.h"
#include "node/exception.h"

LIU_BEGIN

class Object : public Node {
    LIU_DECLARE_2(Object, Node, Node);
public:
    explicit Object(Node *origin = context()->child("Object")) : Node(origin) {}

    Object *init();

    LIU_DECLARE_NATIVE_METHOD(postfix_increment);
    LIU_DECLARE_NATIVE_METHOD(postfix_decrement);

    LIU_DECLARE_NATIVE_METHOD(add_assign);
    LIU_DECLARE_NATIVE_METHOD(subtract_assign);
    LIU_DECLARE_NATIVE_METHOD(multiply_assign);
    LIU_DECLARE_NATIVE_METHOD(divide_assign);
    LIU_DECLARE_NATIVE_METHOD(modulo_assign);
};

LIU_END

#endif // LIU_OBJECT_H
