#include "node/object/language.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

namespace Language {
    Object *root() {
        static Object *_root = NULL;
        if(!_root) {
            _root = Object::root()->fork();
            Object::root()->addChild("Language", _root);
            _root->setNodeOwner(Object::root()); \
            _root->declare("Language");
        }
        return _root;
    }
}

LIU_END
