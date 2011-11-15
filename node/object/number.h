#ifndef LIU_NUMBER_H
#define LIU_NUMBER_H

#include <math.h>

#include "node/object/boolean.h"
#include "node/object/property.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Number : public Object {
    LIU_DECLARE_2(Number, Object, Object);
public:
    explicit Number(Node *origin = context()->child("Object", "Number")) :
        Object(origin), _value(NULL) {}

    static Number *make(const double &value) { return (new Number())->init(&value); }

    Number *init(const double *value = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(double, value, Value);
    LIU_DECLARE_PROPERTY(value);

    virtual bool isDefined(QSet<const Node *> *alreadySeen = NULL) const;

    virtual Node *run(Node *receiver = context());

    virtual bool isEqualTo(const Node *other) const;

    virtual short compare(const Node *other) const;
    short compare(const double &other) const;

    virtual uint hash() const;

    virtual double toDouble(bool *okPtr = NULL) const {
        if(okPtr) *okPtr = true;
        return value();
    };

    virtual QChar toChar(bool *okPtr = NULL) const {
        if(okPtr) *okPtr = true;
        return QChar(int(value()));
    };

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return QString("%1").arg(value(), 0, 'g', 13);
    }

    LIU_DECLARE_NATIVE_METHOD(add);
    LIU_DECLARE_NATIVE_METHOD(subtract);
    LIU_DECLARE_NATIVE_METHOD(multiply);
    LIU_DECLARE_NATIVE_METHOD(divide);
    LIU_DECLARE_NATIVE_METHOD(modulo);

    LIU_DECLARE_NATIVE_METHOD(unary_plus);
    LIU_DECLARE_NATIVE_METHOD(unary_minus);

    LIU_DECLARE_NATIVE_METHOD(prefix_increment);
    LIU_DECLARE_NATIVE_METHOD(prefix_decrement);

    LIU_DECLARE_NATIVE_METHOD(nan);
    LIU_DECLARE_NATIVE_METHOD(inf);

    LIU_DECLARE_NATIVE_METHOD(random);
private:
    double *_value;
};

LIU_END

#endif // LIU_NUMBER_H
