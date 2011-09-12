#ifndef LIU_NUMBER_H
#define LIU_NUMBER_H

#include <math.h>

#include "node/object/element.h"
#include "node/object/boolean.h"
#include "node/object/language/message.h"

LIU_BEGIN

#define LIU_NUMBER(ARGS...) new Number(context()->child("Object", "Number"), ##ARGS)

class Number : public GenericElement<double> {
    LIU_DECLARE(Number, Element, Object);
public:
    explicit Number(Node *origin, const double value = 0) : GenericElement<double>(origin, 0) { setValue(value); }

    Number(const Number &other) : GenericElement<double>(other) { setValue(other.value()); }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Number);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Number, value());

    LIU_DECLARE_NATIVE_METHOD(init) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0, 1);
        if(message->hasInput(0)) {
            bool ok = true;
            double value = message->runFirstInput()->toDouble(message->isQuestioned() ? &ok : NULL);
            if(ok) setValue(value);
        }

//        // === TODO: DRY ===
//        LIU_FIND_LAST_PRIMITIVE;
//        Primitive *nextPrimitive = primitive->next();
//        if(nextPrimitive) {
//            nextPrimitive->run(this);
//            Primitive::skip(this);
//        }

        return this;
    }

    LIU_DECLARE_NATIVE_METHOD(add) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(value() + message->runFirstInput()->toDouble());
    }

    LIU_DECLARE_NATIVE_METHOD(subtract) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(value() - message->runFirstInput()->toDouble());
    }

    LIU_DECLARE_NATIVE_METHOD(multiply) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(value() * message->runFirstInput()->toDouble());
    }

    LIU_DECLARE_NATIVE_METHOD(divide) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(value() / message->runFirstInput()->toDouble());
    }

    LIU_DECLARE_NATIVE_METHOD(modulo) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        long long left = llround(value());
        long long right = llround(message->runFirstInput()->toDouble());
        if(right == 0) LIU_THROW(MathException, "right-hand side cannot be 0");
        return LIU_NUMBER(left % right);
    }

    LIU_DECLARE_NATIVE_METHOD(unary_plus) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        return LIU_NUMBER(value());
    }

    LIU_DECLARE_NATIVE_METHOD(unary_minus) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        return LIU_NUMBER(-value());
    }

    LIU_DECLARE_NATIVE_METHOD(prefix_increment) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        setValue(value() + 1);
        return this;
    }

    LIU_DECLARE_NATIVE_METHOD(prefix_decrement) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        setValue(value() - 1);
        return this;
    }

    LIU_DECLARE_NATIVE_METHOD(nan) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        if(message->isQuestioned())
            return LIU_BOOLEAN(value() != value());
        double zero = 0;
        return LIU_NUMBER(0 / zero);
    }

    LIU_DECLARE_NATIVE_METHOD(inf) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(0);
        if(message->isQuestioned())
            return LIU_BOOLEAN(value() > std::numeric_limits<qreal>::max() ||
                               value() < -std::numeric_limits<qreal>::max());
        double zero = 0;
        return LIU_NUMBER(1 / zero);
    }

    virtual bool isEqualTo(const Node *other) const {
        return value() == Number::cast(other)->value();
    }

    LIU_DECLARE_NATIVE_METHOD(equal_to) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_BOOLEAN(value() == message->runFirstInput()->toDouble());
    }

    virtual short compare(const Node *other) const {
        return compare(Number::cast(other)->value());
    }

    short compare(const double &other) const {
        if(value() > other) return 1;
        else if(value() < other) return -1;
        else return 0;
    }

    LIU_DECLARE_NATIVE_METHOD(compare) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(compare(message->runFirstInput()->toDouble()));
    }

    virtual double toDouble(bool *okPtr = NULL) const {
        Q_UNUSED(okPtr);
        return value();
    };

    virtual QChar toChar() const { return QChar(int(value())); };

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return QString("%1").arg(value(), 0, 'g', 13);
    }
};

LIU_END

#endif // LIU_NUMBER_H
