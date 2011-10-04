#include "node/object/list.h"
#include "node/object/number.h"
#include "node/object/text.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === AbastractList ===

LIU_DEFINE(AbstractList, Object, Object);

void AbstractList::initRoot() {
    LIU_ADD_NATIVE_METHOD(AbstractList, get, []);
    LIU_ADD_NATIVE_METHOD(AbstractList, set, []=);
    LIU_ADD_NATIVE_METHOD(AbstractList, append_or_set, []:=);

    LIU_ADD_NATIVE_METHOD(AbstractList, append);
    LIU_ADD_NATIVE_METHOD(AbstractList, remove, []>>);

    LIU_ADD_NATIVE_METHOD(AbstractList, size);
    LIU_ADD_NATIVE_METHOD(AbstractList, empty);
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, get) { // TODO: use multiple return values (-> index, found)
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = NULL;
    if(Primitive *label = message->firstInput()->label()) {
        Message *msg = Message::dynamicCast(label->value());
        if(msg && msg->name() == "value")
            value = message->runFirstInput();
    }
    bool wasFound = true;
    if(value) {
        int index = get(&value, message->isQuestioned() ? &wasFound : NULL);
        if(wasFound) return LIU_NUMBER(index);
    } else {
        int index = message->runFirstInput()->toDouble();
        value = get(index, message->isQuestioned() ? &wasFound : NULL);
        if(wasFound) return value;
    }
    Primitive::skip(LIU_BOOLEAN(false));
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    int index = message->runFirstInput()->toDouble();
    return set(index, message->runSecondInput());
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, append_or_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1, 2);
    int index;
    if(message->hasASecondInput())
        index = message->runFirstInput()->toDouble();
    else
        index = size();
    Node *value = message->runLastInput();
    if(hasIndex(index))
        return set(index, value);
    else {
        int numBlanksToAppend = index - size();
        for(int i = 1; i <= numBlanksToAppend; ++i) append(LIU_NODE());
        return append(value);
    }
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, append) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = message->runFirstInput();
    append(value);
    return this;
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, remove) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *value = NULL;
    if(Primitive *label = message->firstInput()->label()) { // TODO: DRY!
        Message *msg = Message::dynamicCast(label->value());
        if(msg && msg->name() == "value")
            value = message->runFirstInput();
    }
    int index;
    bool wasFound = true;
    if(value) {
        index = get(&value, message->isQuestioned() ? &wasFound : NULL);
    } else {
        index = message->runFirstInput()->toDouble();
        value = get(index, message->isQuestioned() ? &wasFound : NULL);
    }
    if(!wasFound) Primitive::skip(LIU_BOOLEAN(false));
    remove(index);
    return value;
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, size) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_NUMBER(size());
}

LIU_DEFINE_NATIVE_METHOD(AbstractList, empty) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_QUESTION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(isEmpty());
}

// === OldList ===

LIU_DEFINE(OldList, AbstractList, Object);

void OldList::initRoot() {
    LIU_ADD_NATIVE_METHOD(OldList, init);
    LIU_ADD_NATIVE_METHOD(OldList, make);
}

LIU_DEFINE_NATIVE_METHOD(OldList, init) {
    LIU_FIND_LAST_MESSAGE;
    if(!message->hasAnInput()) return this;
    LIU_CHECK_INPUT_SIZE(1, 2);
    Text *text = Text::dynamicCast(message->runFirstInput());
    if(text) {
        LIU_CHECK_INPUT_SIZE(2);
        QString source = text->value();
        QString separator = message->runSecondInput()->toString();
        QStringList list = source.split(separator);
        QStringListIterator i(list);
        while (i.hasNext()) append(Text::make(i.next()));
        return this;
    }
    LIU_THROW_CONVERSION_EXCEPTION("cannot build a List with these arguments");
}

LIU_DEFINE_NATIVE_METHOD(OldList, make) {
    LIU_FIND_LAST_MESSAGE;
    OldList *list = fork();
    for(int i = 0; i < message->numInputs(); ++i)
        list->append(message->runInput(i));
    return list;
}

LIU_DEFINE(VirtualList, AbstractList, Object);

void VirtualList::initRoot() {
}

LIU_END
