#ifndef LIU_LANGUAGE_NATIVE_METHOD_H
#define LIU_LANGUAGE_NATIVE_METHOD_H

#include "node/object/language/abstractmethod.h"

LIU_BEGIN

namespace Language {
    #define LIU_NATIVE_METHOD(ARGS...) \
    new Language::NativeMethod(context()->child("Object", "Language", "NativeMethod"), ##ARGS)

    #define LIU_ADD_NATIVE_METHOD(CLASS, METHOD, NAME...) \
    addOrSetChild(preferSecondArgumentIfNotEmpty(#METHOD, #NAME), \
        new NativeMethod(NativeMethod::root(), preferSecondArgumentIfNotEmpty(#METHOD, #NAME), \
        static_cast<_MethodPointer_>(&CLASS::_##METHOD##_)))

    typedef Node *(Node::*_MethodPointer_)();

    class NativeMethod : public AbstractMethod {
        LIU_DECLARE(NativeMethod, AbstractMethod, Language);
    public:
        explicit NativeMethod(Node *origin, const QString &name = "", const _MethodPointer_ &method = NULL) :
            AbstractMethod(origin) {
            setNodeName(name);
            setMethod(method);
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(NativeMethod);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(NativeMethod, nodeName(), method());

        _MethodPointer_ method() const { return _method; }

        void setMethod(const _MethodPointer_ &method) {
            _method = method;
            setIsAutoRunnable(method);
        }

        virtual Node *run(Node *receiver = context()) {
            LIU_PUSH_RUN(this);
            return (receiver->*method())();
        }

        virtual QString toString(bool debug = false, short level = 0) const {
            return signature(debug, level);
        }
    private:
        _MethodPointer_ _method;
    };
}

LIU_END

#endif // LIU_LANGUAGE_NATIVE_METHOD_H
