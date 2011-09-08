#include "node/object/boolean.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE(Boolean, Element, Object);

void Boolean::initRoot() {
    LIU_ADD_NATIVE_METHOD(Boolean, equal_to, ==);
}

LIU_END
