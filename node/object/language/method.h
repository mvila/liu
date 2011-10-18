#ifndef LIU_LANGUAGE_METHOD_H
#define LIU_LANGUAGE_METHOD_H

#include "node/object/language/abstractmethod.h"
#include "node/object/language/block.h"

LIU_BEGIN

namespace Language {
    #define LIU_METHOD(ARGS...) \
    new Language::Method(context()->child("Object", "Language", "Method"), ##ARGS)

    class Method : public AbstractMethod {
        LIU_DECLARE(Method, AbstractMethod, Language);
    public:
        explicit Method(Node *origin, Primitive *code = NULL, ParameterList *inputs = NULL, ParameterList *outputs = NULL,
                        const QString &codeInputName = "") :
            AbstractMethod(origin, inputs, outputs, codeInputName), _code(NULL) { setCode(code); }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Method);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Method, LIU_FORK_IF_NOT_NULL(code()), LIU_FORK_IF_NOT_NULL(inputs(false)),
                                           LIU_FORK_IF_NOT_NULL(outputs(false)), codeInputName());

        LIU_DECLARE_NATIVE_METHOD(init);

        Primitive *code() const { return _code; }

        void setCode(Primitive *newCode) {
            if(newCode != _code) {
                if(_code) removeUnnamedChild(_code);
                _code = newCode;
                if(newCode) addUnnamedChild(_code);
                hasChanged();
            }
        }

        virtual void hasChanged() {
            if(code()) {
                setIsAutoRunnable(true);
                Block *block = Block::dynamicCast(code()->value());
                if(block) block->runMetaSections(this);
            }
        }

        virtual Node *run(Node *receiver = context()) {
            Q_UNUSED(receiver);
            runParameters();
            Method *forkedMethod = this; //->fork();
            Node *result = NULL;
            if(code()) {
                try {
                    LIU_PUSH_CONTEXT(forkedMethod);
                    LIU_PUSH_RUN(this);
                    result = code()->run();
                } catch(const Return &ret) {
                    result = ret.result;
                }
            } else
                result = LIU_NODE();
            if(!hasCodeInput())
                return result;
            else
                Primitive::skip(result);
        }

        virtual QString toString(bool debug = false, short level = 0) const {
            QString str = signature(debug, level);
            if(code()) str += " " + code()->toString(debug, level + 1);
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
