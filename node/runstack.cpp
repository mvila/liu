#include "node/runstack.h"

LIU_BEGIN

LIU_DEFINE(RunStack, Node, Node);

void RunStack::initRoot() {
}

void RunStack::dump() const { foreach(Node *run, _stack) run->dump(); }

LIU_END
