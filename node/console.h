#ifndef LIU_CONSOLE_H
#define LIU_CONSOLE_H

#include "node.h"

LIU_BEGIN

class Console : public Node {
    LIU_DECLARE_2(Console, Node, Node);
public:
    explicit Console(Node *origin = context()->child("Console")) : Node(origin) {}

    Console *init();

    LIU_DECLARE_NATIVE_METHOD(write);
    LIU_DECLARE_NATIVE_METHOD(print);
    LIU_DECLARE_NATIVE_METHOD(alert);
    LIU_DECLARE_NATIVE_METHOD(read);
};

LIU_END

#endif // LIU_CONSOLE_H
