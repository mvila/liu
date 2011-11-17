#include "node/object/number.h"
#include "node/object/comparable.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Number, Object, Object);

Number *Number::init(const double *value) {
    Object::init();
    setValue(value);
    return this;
}

Number *Number::initCopy(const Number *other) {
    Object::initCopy(other);
    setValue(other->_value);
    return this;
}

Number::~Number() {
    setValue();
}

void Number::initRoot() {
    addExtension(Comparable::root());

    LIU_ADD_NATIVE_METHOD(Number, init);

    LIU_ADD_PROPERTY(Number, value);

    LIU_ADD_NATIVE_METHOD(Number, add, +);
    LIU_ADD_NATIVE_METHOD(Number, subtract, -);
    LIU_ADD_NATIVE_METHOD(Number, multiply, *);
    LIU_ADD_NATIVE_METHOD(Number, divide, /);
    LIU_ADD_NATIVE_METHOD(Number, modulo, %);

    LIU_ADD_NATIVE_METHOD(Number, unary_plus, unary+);
    LIU_ADD_NATIVE_METHOD(Number, unary_minus, unary-);

    LIU_ADD_NATIVE_METHOD(Number, prefix_increment, prefix++);
    LIU_ADD_NATIVE_METHOD(Number, prefix_decrement, prefix--);

    LIU_ADD_NATIVE_METHOD(Number, nan);
    LIU_ADD_NATIVE_METHOD(Number, inf);

    LIU_ADD_NATIVE_METHOD(Number, random);
}

LIU_DEFINE_NATIVE_METHOD(Number, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasInput(0)) {
        bool ok = true;
        double value = message->runFirstInput()->toDouble(message->isQuestioned() ? &ok : NULL);
        if(ok) setValue(value);
    }
    return this;
}

LIU_DEFINE_ACCESSOR(Number, double, value, Value, 0);

LIU_DEFINE_NATIVE_METHOD(Number, value_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(Number::cast(parent())->hasValue());
    else
        return parent();
}

LIU_DEFINE_NATIVE_METHOD(Number, value_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Number::cast(parent())->setValue(message->runFirstInput()->toDouble());
    return this;
}

bool Number::isDefined(QSet<const Node *> *alreadySeen) const {
    return hasValue() || Object::isDefined(alreadySeen);
}

Node *Number::run(Node *receiver) {
    Q_UNUSED(receiver);
    return Number::make(value());
}

bool Number::isEqualTo(const Node *other) const {
    const Number *otherNumber = Number::dynamicCast(other);
    return otherNumber && value() == otherNumber->value();
}

short Number::compare(const Node *other) const {
    const Number *otherNumber = Number::dynamicCast(other);
    if(otherNumber) return compare(otherNumber->value());
    LIU_THROW(ArgumentException, "a Number is expected");
}

short Number::compare(const double &other) const {
    if(value() > other) return 1;
    else if(value() < other) return -1;
    else return 0;
}

uint Number::hash() const { // FIXME: ugly!
    double val = value();
    double uval = fabs(val);
    uint result;
    if(uval < UINT_MAX / 200 && uval * 100 == ceil(uval * 100)) {
        result = uval * 100;
        if(val < 0) result += UINT_MAX / 2;
    } else
        result = qHash(toString());
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Number, add) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Number::make(value() + message->runFirstInput()->toDouble());
}

LIU_DEFINE_NATIVE_METHOD(Number, subtract) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Number::make(value() - message->runFirstInput()->toDouble());
}

LIU_DEFINE_NATIVE_METHOD(Number, multiply) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Number::make(value() * message->runFirstInput()->toDouble());
}

LIU_DEFINE_NATIVE_METHOD(Number, divide) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return Number::make(value() / message->runFirstInput()->toDouble());
}

LIU_DEFINE_NATIVE_METHOD(Number, modulo) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    long long left = llround(value());
    long long right = llround(message->runFirstInput()->toDouble());
    if(right == 0) LIU_THROW(MathException, "right-hand side cannot be 0");
    return Number::make(left % right);
}

LIU_DEFINE_NATIVE_METHOD(Number, unary_plus) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Number::make(value());
}

LIU_DEFINE_NATIVE_METHOD(Number, unary_minus) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Number::make(-value());
}

LIU_DEFINE_NATIVE_METHOD(Number, prefix_increment) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    setValue(value() + 1);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Number, prefix_decrement) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    setValue(value() - 1);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Number, nan) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(value() != value());
    double zero = 0;
    return Number::make(0 / zero);
}

LIU_DEFINE_NATIVE_METHOD(Number, inf) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(value() > std::numeric_limits<qreal>::max() ||
                           value() < -std::numeric_limits<qreal>::max());
    double zero = 0;
    return Number::make(1 / zero);
}

LIU_DEFINE_NATIVE_METHOD(Number, random) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return Number::make(qrand());
}

LIU_END
