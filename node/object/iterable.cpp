#include "node/object/iterable.h"

LIU_BEGIN

// === Iterable ===

LIU_DEFINE(Iterable, Object, Object);

Iterable::Iterable(Node *origin) : Object(origin) {}

void Iterable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Iterable, init);

    LIU_ADD_NATIVE_METHOD(Iterable, equal_to, ==);
}

LIU_DEFINE_NATIVE_METHOD(Iterable, init) {
    return this;
}

bool Iterable::isEqualTo(const Node *other) const {
    const Iterable *otherIterable = Iterable::dynamicCast(other);
    return otherIterable && false; // [...]
}

LIU_DEFINE_NATIVE_METHOD(Iterable, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(false); // [...]
}

QString Iterable::toString(bool debug, short level) const {
    Q_UNUSED(level);
    return debug ? "\"[...]\"" : "[...]";
}

LIU_END
