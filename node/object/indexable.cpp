#include "node/object/indexable.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE_2(Indexable, Collection, Object);

Indexable *Indexable::init() {
    Collection::init();
    return this;
}

Indexable *Indexable::initCopy(const Indexable *other) {
    Collection::initCopy(other);
    return this;
}

Indexable::~Indexable() {}

void Indexable::initRoot() {
    LIU_ADD_NATIVE_METHOD(Indexable, get, []);
    LIU_ADD_NATIVE_METHOD(Indexable, set, []=);
    LIU_ADD_NATIVE_METHOD(Indexable, append_or_set, []:=);
    LIU_ADD_NATIVE_METHOD(Indexable, unset, []>>);

    LIU_ADD_NATIVE_METHOD(Indexable, index);
}

LIU_DEFINE_NATIVE_METHOD(Indexable, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *index = message->runFirstInput();
    bool wasFound = true;
    Node *result = get(index, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Indexable, set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *index = message->runFirstInput();
    Node *item = message->runSecondInput();
    bool wasFound = true;
    set(index, item, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return this;
}

void Indexable::appendOrSet(Node *index, Node *item, bool *okPtr) {
    bool ok = false;
    if(index)
        set(index, item, &ok);
    if(ok) {
        if(okPtr) *okPtr = true;
    } else {
        append(index, item, okPtr);
    }
}

LIU_DEFINE_NATIVE_METHOD(Indexable, append_or_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1, 2);
    Node *index = message->hasASecondInput() ? message->runFirstInput() : NULL;
    Node *item = message->runLastInput();
    bool wasFound = true;
    appendOrSet(index, item, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Indexable, unset) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *index = message->runFirstInput();
    Node *value = NULL;
    if(Primitive *label = message->firstInput()->label()) { // TODO: DRY!
        Message *msg = Message::dynamicCast(label->value());
        if(msg && msg->name() == "value")
            value = index;
    }
    Node *result;
    bool wasFound = true;
    if(value)
        result = remove(value, message->isQuestioned() ? &wasFound : NULL);
    else
        result = unset(index, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result;
}

Node *Indexable::index(Node *item, bool *wasFoundPtr) {
    Node *result = NULL;
    bool wasFound = false;
    QScopedPointer<Iterator> i(iterator());
    while(i->hasNext()) {
        NodeQPair current = i->next();
        if(current.second->isEqualTo(item)) {
            result = current.first;
            wasFound = true;
            break;
        }
    }
    if(wasFoundPtr)
        *wasFoundPtr = wasFound;
    else if(!wasFound)
        LIU_THROW(NotFoundException, "value not found");
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Indexable, index) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *item = message->runFirstInput();
    bool wasFound = true;
    Node *result = index(item, message->isQuestioned() ? &wasFound : NULL);
    if(!wasFound) Primitive::skip(Boolean::make(false));
    return result;
}

LIU_END
