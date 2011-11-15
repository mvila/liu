#ifndef LIU_CONTROL_FLOW_H
#define LIU_CONTROL_FLOW_H

#include "node.h"
#include "node/exception.h"

LIU_BEGIN

class ControlFlow : public Node {
    LIU_DECLARE_2(ControlFlow, Node, Node);
public:
    explicit ControlFlow(Node *origin = context()->child("ControlFlow")) : Node(origin) {}

    ControlFlow *init();

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
