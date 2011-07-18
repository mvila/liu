#include "tests/objecttest.h"

LIU_BEGIN

void ObjectTest::initialize() {
    QVERIFY(Object::root()->origin() == Node::root());
    QVERIFY(Node::root()->child("Object") == Object::root());
}

LIU_END
