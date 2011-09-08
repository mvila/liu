#ifndef LIU_LANGUAGE_NATIVE_METHOD_H
#define LIU_LANGUAGE_NATIVE_METHOD_H

#include "node/object/language/abstractmethod.h"

LIU_BEGIN

namespace Language {
    #define LIU_NATIVE_METHOD(ARGS...) \
    new Language::NativeMethod(context()->child("Object", "Language", "NativeMethod"), ##ARGS)

    #define LIU_ADD_NATIVE_METHOD(CLASS, METHOD, NAME...) \
    addOrSetChild(preferSecondArgumentIfNotEmpty(#METHOD, #NAME), \
        new NativeMethod(NativeMethod::root(), static_cast<_MethodPointer_>(&CLASS::_##METHOD##_)))

    typedef Node *(Node::*_MethodPointer_)();

    class NativeMethod : public AbstractMethod {
        LIU_DECLARE(NativeMethod, AbstractMethod, Language);
    public:
        explicit NativeMethod(Node *origin, const _MethodPointer_ &method = NULL) :
            AbstractMethod(origin) { setMethod(method); }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(NativeMethod);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(NativeMethod, method());

        _MethodPointer_ method() const { return _method; }

        void setMethod(const _MethodPointer_ &method) {
            _method = method;
            setIsAutoRunnable(method);
        }

        virtual Node *run(Node *receiver = context()) {
            LIU_PUSH_RUN(this);
            return (receiver->*method())();
        }
    private:
        _MethodPointer_ _method;
    };
}

LIU_END

#endif // LIU_LANGUAGE_NATIVE_METHOD_H
