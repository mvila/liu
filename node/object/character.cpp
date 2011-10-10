#include "node/object/character.h"
#include "node/object/text.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE(Character, Element, Object);

void Character::initRoot() {
    LIU_ADD_NATIVE_METHOD(Character, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Character, compare, <=>);
}

bool Character::isEqualTo(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    if(otherText) return value() == otherText->toChar();
    const Character *otherCharacter = Character::dynamicCast(other);
    return otherCharacter && value() == otherCharacter->value();
}

LIU_END
