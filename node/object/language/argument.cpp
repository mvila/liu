#include "node/object/language/message.h"
#include "node/object/language/argument.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Argument, Pair, Language);

    Argument *Argument::init(Primitive *label, Primitive *value) {
        Pair::init(label, value);
        return this;
    }

    Argument *Argument::initCopy(const Argument *other) {
        Pair::initCopy(other);
        return this;
    }

    Argument::~Argument() {}

    void Argument::initRoot() {
        setLabel(Primitive::root());
        setValue(Primitive::root());

        LIU_ADD_PROPERTY(Argument, label);
        LIU_ADD_PROPERTY(Argument, value);
    }

    LIU_DEFINE_NODE_PROPERTY(Argument, Primitive, label, Label);

    QString Argument::labelName() const {
        if(!hasLabel() || label()->hasNext()) LIU_THROW(ArgumentException, "illegal parameter label");
        Message *labelMessage = Message::dynamicCast(label()->value());
        if(!labelMessage) LIU_THROW(ArgumentException, "illegal parameter label");
        return labelMessage->name();
    }

    LIU_DEFINE_NODE_PROPERTY(Argument, Primitive, value, Value);

    QString Argument::toString(bool debug, short level) const {
        return concatenateStrings(hasLabel() ? label()->toString(debug, level) + ":" : "",
                                  " ",
                                  hasValue() ? value()->toString(debug, level) : "");
    }
}

LIU_END
