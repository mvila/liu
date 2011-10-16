#ifndef LIU_DICTIONARY_H
#define LIU_DICTIONARY_H

#include "node/object/list.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

class Dictionary : public List {
    LIU_DECLARE_2(Dictionary, List, Object);
public:
    explicit Dictionary(Node *origin = context()->child("Object", "Dictionary")) :
        List(origin), _indexes(NULL) {}

    Dictionary *init();

    // --- Indexable ---

    virtual void append(Node *index, Node *item, bool *okPtr = NULL);

    // --- Insertable ---

    virtual void insert(Node *index, Node *item, Node *before = NULL, bool *okPtr = NULL);
private:
    QHash<Node::Reference, Node *> *_indexes;
};

LIU_END

#endif // LIU_DICTIONARY_H
