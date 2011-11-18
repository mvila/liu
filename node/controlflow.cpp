#include "node/controlflow.h"
#include "node/object/boolean.h"
#include "node/object/language/message.h"
#include "node/object/language/block.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(ControlFlow, Node, Node);

ControlFlow *ControlFlow::init() {
    Node::init();
    return this;
}

ControlFlow *ControlFlow::initCopy(const ControlFlow *other) {
    Node::initCopy(other);
    return this;
}

ControlFlow::~ControlFlow() {}

void ControlFlow::initRoot() {
    LIU_ADD_NATIVE_METHOD(ControlFlow, if);
    LIU_ADD_NATIVE_METHOD(ControlFlow, unless);

    LIU_ADD_NATIVE_METHOD(ControlFlow, loop);

    LIU_ADD_NATIVE_METHOD(ControlFlow, while);
    LIU_ADD_NATIVE_METHOD(ControlFlow, until);

    LIU_ADD_NATIVE_METHOD(ControlFlow, break);
    LIU_ADD_NATIVE_METHOD(ControlFlow, continue);
}

Node *ControlFlow::ifOrUnless(bool isIf) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1, 3);
    Node *testResult = message->runFirstInput();
    if(message->hasASecondInput()) {
        if(testResult->toBool() == isIf) return message->runSecondInput(this);
        if(message->hasAThirdInput()) return message->runThirdInput(this);
        return testResult;
    }
    LIU_FIND_LAST_PRIMITIVE;
    if(Primitive *nextPrimitive = primitive->hasNext()) {
        if(testResult->toBool() == isIf) Primitive::skip(nextPrimitive->run(this));
        if(Block *block = Block::dynamicCast(nextPrimitive->value()))
            if(Section *elseSection = block->section("else")) Primitive::skip(elseSection->run(this));
        Primitive::skip(testResult);
    }
    LIU_THROW(InterpreterException, QString("missing code after an %1 statement").arg(isIf ? "if" : "unless"));
}

LIU_DEFINE_NATIVE_METHOD(ControlFlow, loop) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->hasNext();
    if(!nextPrimitive)
        LIU_THROW(InterpreterException, "missing code after a loop statement");
    Block *block = Block::dynamicCast(nextPrimitive->value());
    Section *between = block ? block->section("between") : NULL;
    HugeInteger count;
    if(message->hasAnInput()) { // finite loop
        count = message->runFirstInput()->toDouble();
        if(count < 0) LIU_THROW(ArgumentException, "loop count must be greater (or equal) than 0");
    } else
        count = -1; // infinite loop
    Node *result = NULL;
    if(count != 0) {
        try {
            HugeInteger i = 0;
            bool first = true;
            while(true) {
                if(count > 0 && i == count) break;
                if(!first) {
                    if(between) try { between->run(); } catch(Continue) {}
                } else
                    first = false;
                try { result = nextPrimitive->run(); } catch(Continue) {}
                i++;
            }
        } catch(const Break &brk) {
            result = brk.result;
        }
    } else
        result = LIU_NODE();
    Primitive::skip(result);
}

Node *ControlFlow::whileOrUntil(bool isWhile) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->hasNext();
    if(!nextPrimitive)
        LIU_THROW(InterpreterException, QString("missing code after %1 statement").arg(isWhile ? "a while" : "an until"));
    Block *block = Block::dynamicCast(nextPrimitive->value());
    Section *between = block ? block->section("between") : NULL;
    Node *result = NULL;
    try {
        Node *test = NULL;
        bool first = true;
        while(true) {
            if(isWhile) {
                test = message->runFirstInput();
                if(test->toBool()) result = test; else break;
            }
            if(!first) {
                if(between) try { between->run(); } catch(Continue) {}
            } else
                first = false;
            try { result = nextPrimitive->run(); } catch(Continue) {}
            if(!isWhile) {
                test = message->runFirstInput();
                if(test->toBool()) break;
            }
        }
    } catch(const Break &brk) {
        result = brk.result;
    }
    if(!result) result = LIU_NODE();
    Primitive::skip(result);
}

LIU_DEFINE_NATIVE_METHOD(ControlFlow, break) {
    LIU_FIND_LAST_PRIMITIVE;
    if(primitive->hasNext())
        LIU_THROW(InterpreterException, "dead code found after a break statement");
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    Node *result = message->hasAnInput() ? message->runFirstInput() : LIU_NODE();
    throw Break(result);
}

LIU_DEFINE_NATIVE_METHOD(ControlFlow, continue) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    throw Continue();
}

LIU_END
