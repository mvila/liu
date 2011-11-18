#include "node/object/language/abstractmethod.h"
#include "node/object/language/message.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(AbstractMethod, Object, Language);

    AbstractMethod *AbstractMethod::init(ParameterList *inputs,
                                         ParameterList *outputs,
                                         const QString *codeInputName) {
        Object::init();
        setInputs(inputs);
        setOutputs(outputs);
        setCodeInputName(codeInputName);
        return this;
    }

    AbstractMethod *AbstractMethod::initCopy(const AbstractMethod *other) {
        Object::initCopy(other);
        setInputs(other->_inputs);
        setOutputs(other->_outputs);
        setCodeInputName(other->_codeInputName);
        return this;
    }

    AbstractMethod::~AbstractMethod() {
        setInputs();
        setOutputs();
        setCodeInputName();
    }

    void AbstractMethod::initRoot() {
        setInputs(ParameterList::root());
        setOutputs(ParameterList::root());
    }

    LIU_DEFINE_NODE_ACCESSOR(AbstractMethod, ParameterList, inputs, Inputs);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(AbstractMethod, inputs);

    void AbstractMethod::appendInput(Argument *argument) {
        Primitive *label = argument->hasLabel();
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

    LIU_DEFINE_NODE_ACCESSOR(AbstractMethod, ParameterList, outputs, Outputs);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(AbstractMethod, outputs);

    LIU_DEFINE_ACCESSOR(AbstractMethod, QString, codeInputName, CodeInputName,);

    void AbstractMethod::runParameters() {
        return;
        LIU_FIND_LAST_MESSAGE;
//        P(message->name());
//        P(inputs()->size());
//        P(inputs()->toString());
        AbstractMethod *forkedMethod = this; //->fork();
        QHash<QString, Parameter *> labels(inputs()->labels());
        if(message->inputs(false)) {
            ArgumentBunch::Iterator iterator(message->inputs());
            short i = -1;
            bool labelFound = false;
            while(Argument *argument = iterator.next()) {
                Parameter *parameter;
                if(argument->hasLabel()) {
                    QString labelName = argument->labelName();
                    if(!hasInput(labelName)) LIU_THROW(NotFoundException, QString("unknown parameter label '%1'").arg(labelName));
                    if(!labels.contains(labelName)) LIU_THROW(DuplicateException, QString("duplicated parameter label '%1'").arg(labelName));
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
        if(hasCodeInputName()) {
            LIU_FIND_LAST_PRIMITIVE;
            Primitive *nextPrimitive = primitive->hasNext();
            forkedMethod->addOrSetChild(codeInputName(), nextPrimitive ? nextPrimitive : Primitive::make()); // FIXME: abstract primitive?
        }
    }
}

LIU_END
