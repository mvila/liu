#ifndef LIU_CONSOLE_H
#define LIU_CONSOLE_H

#include "node.h"

LIU_BEGIN

#define LIU_CONSOLE(ARGS...) new Console(context()->child("Console"), ##ARGS)

class Console : public Node {
    LIU_DECLARE(Console, Node, Node);
public:
    explicit Console(Node *origin) : Node(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Console);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Console);

    LIU_DECLARE_NATIVE_METHOD(write);
    LIU_DECLARE_NATIVE_METHOD(print);
    LIU_DECLARE_NATIVE_METHOD(alert);
    LIU_DECLARE_NATIVE_METHOD(read);
};

LIU_END

#endif // LIU_CONSOLE_H
