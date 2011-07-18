#ifndef LIU_CHARACTER_H
#define LIU_CHARACTER_H

#include "node/object/element.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/message.h"

LIU_BEGIN

#define LIU_CHARACTER(ARGS...) new Character(context()->child("Object", "Character"), ##ARGS)

class Character : public GenericElement<QChar> {
    LIU_DECLARE(Character, Element, Object);
public:
    explicit Character(Node *origin, const QChar &value = QChar::Null) :
        GenericElement<QChar>(origin, QChar::Null) { setValue(value); }

    Character(const Character &other) : GenericElement<QChar>(other) { setValue(other.value()); }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Character);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Character, value());

    virtual bool isEqualTo(const Node *other) const {
        return value() == cast(other)->value();
    }

    LIU_DECLARE_NATIVE_METHOD(equal_to) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_BOOLEAN(value() == message->runFirstInput()->toChar());
    }

    virtual short compare(const Node *other) const {
        return compare(cast(other)->value());
    }

    short compare(const QChar &other) const {
        if(value() > other) return 1;
        else if(value() < other) return -1;
        else return 0;
    }

    LIU_DECLARE_NATIVE_METHOD(compare) {
        LIU_FIND_LAST_MESSAGE;
        LIU_CHECK_INPUT_SIZE(1);
        return LIU_NUMBER(compare(message->runFirstInput()->toChar()));
    }

    virtual double toDouble(bool *okPtr = NULL) const {
        Q_UNUSED(okPtr);
        return value().unicode();
    };

    virtual QChar toChar() const { return value(); };

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(level);
        return debug ? ("'" + QString(value()) + "'") : value();
    }
};

LIU_END

#endif // LIU_CHARACTER_H
