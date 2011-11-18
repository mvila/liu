#ifndef LIU_LANGUAGE_METHOD_H
#define LIU_LANGUAGE_METHOD_H

#include "node/object/language/abstractmethod.h"
#include "node/object/language/block.h"

LIU_BEGIN

namespace Language {
    class Method : public AbstractMethod {
        LIU_DECLARE_2(Method, AbstractMethod, Language);
    public:
        explicit Method(Node *origin = context()->child("Object", "Language", "Method")) :
            AbstractMethod(origin), _code(NULL) {}

        static Method *make(Primitive *code) { return (new Method())->init(code); }

        Method *init(Primitive *code = NULL);

        LIU_DECLARE_NODE_ACCESSOR(Primitive, code, Code);

        LIU_DECLARE_NATIVE_METHOD(init);

        virtual Node *run(Node *receiver = context()) {
            Q_UNUSED(receiver);
            runParameters();
            Method *forkedMethod = this; //->fork();
            Node *result = NULL;
            if(hasCode()) {
                try {
                    LIU_PUSH_CONTEXT(forkedMethod);
                    LIU_PUSH_RUN(this);
                    result = code()->run();
                } catch(const Return &ret) {
                    result = ret.result;
                }
            } else
                result = LIU_NODE();
            if(!hasCodeInputName())
                return result;
            else
                Primitive::skip(result);
        }

        virtual QString toString(bool debug = false, short level = 0) const {
            QString str = signature(debug, level);
            if(hasCode()) str += " " + code()->toString(debug, level + 1);
            return str;
        }
    private:
        Primitive *_code;
    public:
        // === Return ===

        class Return {
        public:
            Return(Node *result = NULL) : result(result) {}

            Node *result;
        };

        LIU_DECLARE_NATIVE_METHOD(return);
    };
}

LIU_END

#endif // LIU_LANGUAGE_METHOD_H
