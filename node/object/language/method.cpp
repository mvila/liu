#include "node/object/language/method.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Method, AbstractMethod, Language);

    Method *Method::init(Primitive *code) {
        AbstractMethod::init();
        setCode(code);
        return this;
    }

    Method *Method::initCopy(const Method *other) {
        AbstractMethod::initCopy(other);
        setCode(other->_code);
        return this;
    }

    Method::~Method() {
        setCode();
    }

    void Method::initRoot() {
        setCode(Primitive::root());

        LIU_ADD_NATIVE_METHOD(Method, init);

        LIU_ADD_NATIVE_METHOD(Method, return);
    }

    LIU_DEFINE_NODE_ACCESSOR(Method, Primitive, code, Code);

    void Method::codeWillChange() {}

    void Method::codeHasChanged() {
        if(_code) {
            setIsAutoRunnable(true);
            Block *block = Block::dynamicCast(code()->value());
            if(block) block->runMetaSections(this);
        }
    }

    LIU_DEFINE_NATIVE_METHOD(Method, init) {
        LIU_FIND_LAST_MESSAGE;
        if(message->hasAnInput()) {
            Primitive *primitive = Primitive::dynamicCast(message->runFirstInput());
            if(!primitive) LIU_THROW(ArgumentException, "code parameter must be a Primitive");
            setCode(primitive);
            if(message->hasASecondInput()) {
                OldList *list = OldList::dynamicCast(message->runSecondInput());
                if(!list) LIU_THROW(ArgumentException, "inputs parameter must be a List");
    //            setInputs(list);
            }
        } else {
            LIU_FIND_LAST_PRIMITIVE;
            Primitive *nextPrimitive = primitive->hasNext();
            if(nextPrimitive) {
                setCode(nextPrimitive);
                Primitive::skip(this);
            }
        }
        return this;
    }

    LIU_DEFINE_NATIVE_METHOD(Method, return) {
        LIU_FIND_LAST_PRIMITIVE;
        if(primitive->hasNext())
            LIU_THROW(InterpreterException, "dead code found after a return statement");
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0, 1);
        Node *result = message->hasInput(0) ? message->runFirstInput() : LIU_NODE();
        throw Return(result);
    }
}

LIU_END
