#include "node/object/character.h"

LIU_BEGIN

LIU_DEFINE(Character, Element, Object);

void Character::initRoot() {
    LIU_ADD_NATIVE_METHOD(Character, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Character, compare, <=>);
}

LIU_END
