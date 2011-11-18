#ifndef LIU_LANGUAGE_NATIVE_METHOD_H
#define LIU_LANGUAGE_NATIVE_METHOD_H

#include "node/object/language/abstractmethod.h"

LIU_BEGIN

namespace Language {
    #define LIU_ADD_NATIVE_METHOD(CLASS, METHOD, NAME...) \
    addOrSetChild(preferSecondArgumentIfNotEmpty(#METHOD, #NAME), \
        (new Language::NativeMethod(NativeMethod::root()))->init(&preferSecondArgumentIfNotEmpty(#METHOD, #NAME), \
        static_cast<_MethodPointer_>(&CLASS::_##METHOD##_)))

    typedef Node *(Node::*_MethodPointer_)();

    class NativeMethod : public AbstractMethod {
        LIU_DECLARE_2(NativeMethod, AbstractMethod, Language);
    public:
        explicit NativeMethod(Node *origin = context()->child("Object", "Language", "NativeMethod")) :
            AbstractMethod(origin), _method(NULL) {}

        NativeMethod *init(const QString *name = NULL, const _MethodPointer_ method = NULL);

        _MethodPointer_ method() const { return _method; }

        void setMethod(const _MethodPointer_ &method) {
            _method = method;
            setIsAutoRunnable(method);
        }

        virtual Node *run(Node *receiver = context()) {
            runParameters();
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
