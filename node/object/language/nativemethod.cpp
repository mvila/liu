#include "node/object/language/nativemethod.h"
#include "node/object/language/message.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(NativeMethod, AbstractMethod, Language);

    NativeMethod *NativeMethod::init(const QString *name, const _MethodPointer_ method) {
        AbstractMethod::init();
        if(name) setNodeName(*name);
        if(method)
            setMethod(method);
        else {
            NativeMethod *orig = NativeMethod::dynamicCast(origin());
            if(orig && orig->_method) setMethod(orig->_method);
        }
        return this;
    }

    NativeMethod *NativeMethod::initCopy(const NativeMethod *other) {
        AbstractMethod::initCopy(other);
        setNodeName(other->nodeName());
        setMethod(other->_method);
        return this;
    }

    NativeMethod::~NativeMethod() {}

    void NativeMethod::initRoot() {}
}

LIU_END
