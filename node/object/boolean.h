#ifndef LIU_BOOLEAN_H
#define LIU_BOOLEAN_H

#include "node/object/element.h"
#include "node/object/language/message.h"

LIU_BEGIN

#define LIU_BOOLEAN(ARGS...) new Boolean(context()->child("Object", "Boolean"), ##ARGS)

class Boolean : public GenericElement<bool> {
    LIU_DECLARE(Boolean, Element, Object);
public:
    explicit Boolean(Node *origin, bool value = false) : GenericElement<bool>(origin, false) { setValue(value); }

    Boolean(const Boolean &other) : GenericElement<bool>(other) { setValue(other.value()); }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Boolean);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Boolean, value());

    virtual bool isEqualTo(const Node *other) const {
        return value() == Boolean::cast(other)->value();
    }

    LIU_DECLARE_NATIVE_METHOD(equal_to) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_BOOLEAN(value() == message->runFirstInput()->toBool());
    }

    virtual bool toBool() const { return value(); };

    virtual double toDouble(bool *okPtr = NULL) const {
        Q_UNUSED(okPtr);
        return value() ? 1 : 0;
    }

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return value() ? "yes" : "no";
    }
};

LIU_END

#endif // LIU_BOOLEAN_H
