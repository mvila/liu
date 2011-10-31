#ifndef LIU_CHARACTER_H
#define LIU_CHARACTER_H

#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Character : public Object {
    LIU_DECLARE_2(Character, Object, Object);
public:
    explicit Character(Node *origin = context()->child("Object", "Character")) :
        Object(origin), _value(NULL) {}

    static Character *make(const QChar &value) { return (new Character())->init(&value); }

    Character *init(const QChar *value = NULL);

    LIU_DECLARE_NATIVE_METHOD(init);

    LIU_DECLARE_ACCESSOR(QChar, value, Value);
    LIU_DECLARE_PROPERTY(value);

    virtual bool isDefined(QSet<const Node *> *alreadySeen = NULL) const;

    virtual bool isEqualTo(const Node *other) const;

    virtual short compare(const Node *other) const;
    short compare(const QChar &other) const;

    virtual uint hash() const { return qHash(value()); }

    virtual double toDouble(bool *okPtr = NULL) const {
        if(okPtr) *okPtr = true;
        return value().unicode();
    };

    virtual QChar toChar(bool *okPtr = NULL) const {
        if(okPtr) *okPtr = true;
        return value();
    };

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(level);
        return debug ? ("'" + QString(value()) + "'") : value();
    }
private:
    QChar *_value;
};

LIU_END

#endif // LIU_CHARACTER_H
