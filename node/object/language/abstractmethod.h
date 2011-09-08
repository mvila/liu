#ifndef LIU_LANGUAGE_ABSTRACT_METHOD_H
#define LIU_LANGUAGE_ABSTRACT_METHOD_H

#include "node/object.h"

LIU_BEGIN

namespace Language {
    #define LIU_ABSTRACT_METHOD(ARGS...) \
    new Language::AbstractMethod(context()->child("Object", "Language", "AbstractMethod"), ##ARGS)

    class AbstractMethod : public Object {
        LIU_DECLARE(AbstractMethod, Object, Language);
    public:
        explicit AbstractMethod(Node *origin) : Object(origin) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(AbstractMethod);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(AbstractMethod);
    };
}

LIU_END

#endif // LIU_LANGUAGE_ABSTRACT_METHOD_H
