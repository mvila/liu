#include "node/object/character.h"
#include "node/object/comparable.h"
#include "node/object/text.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Character, Object, Object);

Character *Character::init(const QChar *value) {
    Object::init();
    setValue(value);
    return this;
}

Character *Character::initCopy(const Character *other) {
    Object::initCopy(other);
    setValue(other->_value);
    return this;
}

Character::~Character() {
    setValue();
}

void Character::initRoot() {
    addExtension(Comparable::root());

    LIU_ADD_NATIVE_METHOD(Character, init);

    LIU_ADD_PROPERTY(Character, value);
}

LIU_DEFINE_NATIVE_METHOD(Character, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasInput(0)) {
        bool ok = true;
        QChar value = message->runFirstInput()->toChar(message->isQuestioned() ? &ok : NULL);
        if(ok) setValue(value);
    }
    return this;
}

LIU_DEFINE_ACCESSOR(Character, QChar, value, Value,);

LIU_DEFINE_NATIVE_METHOD(Character, value_get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(message->isQuestioned())
        return Boolean::make(Character::cast(parent())->hasValue());
    else
        return parent();
}

LIU_DEFINE_NATIVE_METHOD(Character, value_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Character::cast(parent())->setValue(message->runFirstInput()->toChar());
    return this;
}

bool Character::isEqualTo(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    if(otherText) return value() == otherText->toChar();
    const Character *otherCharacter = Character::dynamicCast(other);
    if(otherCharacter) return value() == otherCharacter->value();
    LIU_THROW(ArgumentException, "a Text or a Character is expected");
}

short Character::compare(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    if(otherText) return compare(otherText->toChar());
    const Character *otherCharacter = Character::dynamicCast(other);
    if(otherCharacter) return compare(otherCharacter->value());
    LIU_THROW(ArgumentException, "a Text or a Character is expected");
}

short Character::compare(const QChar &other) const {
    if(value() > other) return 1;
    else if(value() < other) return -1;
    else return 0;
}

LIU_END
