#ifndef LIU_CONTROL_FLOW_H
#define LIU_CONTROL_FLOW_H

#include "node.h"
#include "node/exception.h"

LIU_BEGIN

#define LIU_CONTROL_FLOW(ARGS...) new ControlFlow(context()->child("ControlFlow"), ##ARGS)

class ControlFlow : public Node {
    LIU_DECLARE(ControlFlow, Node, Node);
public:
    explicit ControlFlow(Node *origin) : Node(origin) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(ControlFlow);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(ControlFlow);

    LIU_DECLARE_NATIVE_METHOD(if) { return ifOrUnless(true); }
    LIU_DECLARE_NATIVE_METHOD(unless) { return ifOrUnless(false); }
private:
    Node *ifOrUnless(bool isIf);
public:

    LIU_DECLARE_NATIVE_METHOD(loop);

    LIU_DECLARE_NATIVE_METHOD(while) { return whileOrUntil(true); }
    LIU_DECLARE_NATIVE_METHOD(until) { return whileOrUntil(false); }
private:
    Node *whileOrUntil(bool isWhile);
public:
    // === Break ===

    class Break {
    public:
        Break(Node *result = NULL) : result(result) {}

        Node *result;
    };

    LIU_DECLARE_NATIVE_METHOD(break);

    // === Continue ===

    class Continue {
    public:
        Continue() {}
    };

    LIU_DECLARE_NATIVE_METHOD(continue);
};

LIU_END

#endif // LIU_CONTROL_FLOW_H
