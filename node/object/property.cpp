#include "node/object/property.h"

LIU_BEGIN

LIU_DEFINE_2(Property, Object, Object);

Property *Property::init() {
    Object::init();
    setIsAutoRunnable(true);
    return this;
}

Property *Property::initCopy(const Property *other) {
    Object::initCopy(other);
    return this;
}

Property::~Property() {}

void Property::initRoot() {}

LIU_END
