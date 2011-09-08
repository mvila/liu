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
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Method, LIU_FORK_IF_NOT_NULL(code()));

        LIU_DECLARE_NATIVE_METHOD(init);

        Primitive *code() const { return _code; }

        void setCode(Primitive *newCode) {
            if(newCode != _code) {
                if(_code) removeAnonymousChild(_code);
                _code = newCode;
                if(newCode) addAnonymousChild(_code);
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
            LIU_FIND_LAST_MESSAGE;
            Method *forkedMethod = this; //->fork();
            QHash<QString, Parameter *> labels(inputs()->labels());
            if(message->inputs(false)) {
                ArgumentBunch::Iterator iterator(message->inputs());
                short i = -1;
                bool labelFound = false;
                while(Argument *argument = iterator.next()) {
                    Parameter *parameter;
                    if(argument->label()) {
                        QString labelName = argument->labelName();
                        if(!hasInput(labelName)) LIU_THROW(NotFoundException, "unknown parameter label");
                        if(!labels.contains(labelName)) LIU_THROW(DuplicateException, "duplicated parameter label");
                        parameter = input(labelName);
                        labelFound = true;
                    } else {
                        if(labelFound) LIU_THROW(ArgumentException, "positional arguments are forbidden after labeled ones");
                        i++;
                        if(!hasInput(i)) LIU_THROW(IndexOutOfBoundsException, "too many arguments");
                        parameter = input(i);
                    }
                    Node *rcvr = parameter->isParented() ? forkedMethod->parent() : forkedMethod;
                    Node *val = parameter->isEscaped() ? argument->value() : argument->run();
                    rcvr->addOrSetChild(parameter->label(), val);
                    labels.remove(parameter->label());
                }
            }
            foreach(Parameter *parameter, labels) {
                if(!parameter->defaultValue()) LIU_THROW(ArgumentException, "missing mandatory parameter");
                Node *rcvr = parameter->isParented() ? forkedMethod->parent() : forkedMethod;
                Node *val = parameter->isEscaped() ? parameter->defaultValue() : parameter->run();
                rcvr->addOrSetChild(parameter->label(), val);
            }
            if(hasCodeInput()) {
                LIU_FIND_LAST_PRIMITIVE;
                Primitive *nextPrimitive = primitive->next();
                forkedMethod->addOrSetChild(codeInputName(), nextPrimitive ? nextPrimitive : LIU_PRIMITIVE()); // FIXME: abstract primitive?
            }
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
