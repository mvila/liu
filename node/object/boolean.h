#ifndef LIU_BOOLEAN_H
#define LIU_BOOLEAN_H

#include "node/object/property.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Boolean : public Object {
    LIU_DECLARE_2(Boolean, Object, Object);
public:
    explicit Boolean(Node *origin = context()->child("Object", "Boolean")) :
        Object(origin), _value(NULL) {}

    static Boolean *make(const bool &value) { return (new Boolean())->init(&value); }

    Boolean *init(const bool *value = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(bool, value, Value);
    LIU_DECLARE_PROPERTY(value);

    virtual bool isEqualTo(const Node *other) const;

    virtual bool toBool() const { bool *value = hasValue(); return value && *value; };

    virtual double toDouble(bool *okPtr = NULL) const {
        Q_UNUSED(okPtr);
        return value() ? 1 : 0;
    }

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return value() ? "yes" : "no";
    }
private:
    bool *_value;
};

LIU_END

#endif // LIU_BOOLEAN_H
