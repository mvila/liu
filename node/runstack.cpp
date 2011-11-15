#include "node/runstack.h"

LIU_BEGIN

LIU_DEFINE_2(RunStack, Node, Node);

RunStack *RunStack::init() {
    Node::init();
    return this;
}

RunStack *RunStack::initCopy(const RunStack *other) {
    Node::initCopy(other);
    setStack(other->_stack);
    return this;
}

RunStack::~RunStack() {
    setStack();
}

void RunStack::initRoot() {}

LIU_DEFINE_ACCESSOR(RunStack, QStack<Node *>, stack, Stack,);

void RunStack::dump() const { foreach(Node *run, stack()) run->dump(); }

LIU_END
