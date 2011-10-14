#include "node/object/dictionary.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Dictionary ===

LIU_DEFINE_2(Dictionary, List, Object);

Dictionary *Dictionary::init() {
    List::init();
    return this;
}

Dictionary *Dictionary::initCopy(const Dictionary *other) {
    List::initCopy(other);
    return this;
}

Dictionary::~Dictionary() {}

void Dictionary::initRoot() {}

LIU_END
