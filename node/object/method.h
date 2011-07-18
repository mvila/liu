#ifndef LIU_METHOD_H
#define LIU_METHOD_H

#include "node/object/element.h"
#include "node/object/message.h"
#include "node/object/language/block.h"
#include "node/object/language/parameterlist.h"

LIU_BEGIN

#define LIU_METHOD(ARGS...) new Method(context()->child("Object", "Method"), ##ARGS)

class Method : public GenericNodeElement<Primitive> {
    LIU_DECLARE(Method, Element, Object);
public:
    explicit Method(Node *origin, Primitive *code = NULL, ParameterList *inputs = NULL, ParameterList *outputs = NULL,
                    const QString &codeInputName = "") :
        GenericNodeElement<Primitive>(origin), _inputs(inputs), _outputs(outputs),
        _codeInputName(codeInputName) { setCode(code); }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Method);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Method, LIU_FORK_IF_NOT_NULL(code()), LIU_FORK_IF_NOT_NULL(inputs(false)),
                                         LIU_FORK_IF_NOT_NULL(outputs(false)), codeInputName());

    LIU_DECLARE_NATIVE_METHOD(init);

    Primitive *code() const { return value(); } // aliases
    void setCode(Primitive *code) { setValue(code); }

    virtual void hasChanged() {
        if(code()) {
            setIsAutoRunnable(true);
            Block *block = Block::dynamicCast(code()->value());
            if(block) block->runMetaSections(this);
        }
    }

    ParameterList *inputs(bool createIfNull = true) const {
        if(!_inputs && createIfNull) constCast(this)->_inputs = LIU_PARAMETER_LIST();
        return _inputs;
    }

    void setInputs(ParameterList *inputs) { _inputs = inputs; }

    void setInputs(ArgumentBunch *arguments) {
        if(hasAnInput()) inputs()->clear();
        ArgumentBunch::Iterator i(arguments);
        while(Argument *argument = i.next())
            appendInput(argument);
    }

//    void setInputs(List *list) {
//        if(hasAnInput()) inputs()->clear();
//        List::Iterator i(list);
//        while(Node *value = i.next())
//            appendInput(LIU_ARGUMENT(value));
//    }

    Parameter *input(short i) const { return inputs(false)->get(i); }
    Parameter *input(const QString &label) const { return inputs(false)->get(label); }
    bool hasInput(short i) const { return inputs(false) && inputs()->hasIndex(i); }
    bool hasInput(const QString &label) const { return inputs(false) && inputs()->hasLabel(label); }
    bool hasAnInput() const { return hasInput(0); }

    void appendInput(Argument *argument) {
        Primitive *label = argument->label();
        Primitive *defaultValue = argument->value();
        if(!label) {
            label = defaultValue;
            defaultValue = NULL;
        }
        if(label->hasNext())
            LIU_THROW(ArgumentException, "illegal label parameter found in method definition (should be a Message");
        Message *labelMsg = Message::dynamicCast(label->value());
        if(!labelMsg)
            LIU_THROW(ArgumentException, "illegal label parameter found in method definition (should be a Message)");
        inputs()->append(LIU_PARAMETER(labelMsg->name(), defaultValue,
                                         labelMsg->isEscaped(), labelMsg->isParented()));
    }

    ParameterList *outputs(bool createIfNull = true) const {
        if(!_outputs && createIfNull) constCast(this)->_outputs = LIU_PARAMETER_LIST();
        return _outputs;
    }

    void setOutputs(ParameterList *outputs) { _outputs = outputs; }

    const QString &codeInputName() const { return _codeInputName; }
    void setCodeInputName(const QString &name) { _codeInputName = name; }
    bool hasCodeInput() const { return !_codeInputName.isEmpty(); }

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

    virtual void hasBeenDefined(Message *message) {
        setNodeName(message->name());
        if(message->hasAnInput()) setInputs(message->inputs());
        if(message->hasCodeInput()) setCodeInputName(message->codeInputName());
    }

    virtual QString toString(bool debug = false, short level = 0) const {
        QString str = nodeName();
        str += "(";
        if(inputs(false)) str += inputs()->toString(debug, level);
        str += ")";
        if(hasCodeInput()) str += " " + codeInputName() + "...";
        if(outputs(false) && outputs()->isNotEmpty()) str += " -> " + outputs()->toString(debug, level);
        if(code()) str += " " + code()->toString(debug, level + 1);
        return str;
    }
private:
    ParameterList *_inputs;
    ParameterList *_outputs;
    QString _codeInputName;
public:
    // === Return ===

    class Return {
    public:
        Return(Node *result = NULL) : result(result) {}

        Node *result;
    };

    LIU_DECLARE_NATIVE_METHOD(return);
};

LIU_END

#endif // LIU_METHOD_H
