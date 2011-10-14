#ifndef LIU_DICTIONARY_H
#define LIU_DICTIONARY_H

#include "node/object/list.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Dictionary : public List {
    LIU_DECLARE(Dictionary, List, Object);
public:
    explicit Dictionary(Node *origin = context()->child("Object", "Dictionary")) :
        List(origin) {}

    static Dictionary *make() { return (new Dictionary())->init(); }

    Dictionary *init();
    Dictionary *initCopy(const Dictionary *other);

    virtual ~Dictionary();

    LIU_DECLARE_AND_DEFINE_FORK_METHOD_2(Dictionary);
    LIU_DECLARE_AND_DEFINE_COPY_METHOD_2(Dictionary);
};

LIU_END

#endif // LIU_DICTIONARY_H
