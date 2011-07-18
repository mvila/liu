#ifndef LIU_LANGUAGE_SOURCE_CODE_DICTIONARY_H
#define LIU_LANGUAGE_SOURCE_CODE_DICTIONARY_H

#include "node/object/dictionary.h"
#include "node/object/language/sourcecode.h"

LIU_BEGIN

namespace Language {
    #define LIU_SOURCE_CODE_DICTIONARY(ARGS...) \
    new Language::SourceCodeDictionary(context()->child("Object", "Language", "SourceCodeDictionary"), ##ARGS)

    class SourceCodeDictionary : public GenericDictionary<Node::Reference, SourceCode *> {
        LIU_DECLARE(SourceCodeDictionary, Dictionary, Language);
    public:
        SourceCodeDictionary(Node *origin) :
            GenericDictionary<Node::Reference, SourceCode *>(origin) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(SourceCodeDictionary);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(SourceCodeDictionary);
    };
}

LIU_END

#endif // LIU_LANGUAGE_SOURCE_CODE_DICTIONARY_H
