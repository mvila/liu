#ifndef LIU_OBJECT_H
#define LIU_OBJECT_H

#include "node.h"
#include "node/exception.h"

LIU_BEGIN

#define LIU_OBJECT(ARGS...) new Object(context()->child("Object"), ##ARGS)

class Object : public Node {
    LIU_DECLARE(Object, Node, Node);
public:
    explicit Object(Node *origin) : Node(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Object);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Object);

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
