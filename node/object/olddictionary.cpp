#include "node/object/olddictionary.h"
#include "node/object/text.h"
#include "node/object/language/message.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

LIU_DEFINE(OldDictionary, Object, Object);

void OldDictionary::initRoot() {
    LIU_ADD_NATIVE_METHOD(OldDictionary, init);
    LIU_ADD_NATIVE_METHOD(OldDictionary, make);

    LIU_ADD_NATIVE_METHOD(OldDictionary, get, []);
    LIU_ADD_NATIVE_METHOD(OldDictionary, set, []=);
    LIU_ADD_NATIVE_METHOD(OldDictionary, append_or_set, []:=);

    LIU_ADD_NATIVE_METHOD(OldDictionary, size);
    LIU_ADD_NATIVE_METHOD(OldDictionary, empty);

    LIU_ADD_NATIVE_METHOD(OldDictionary, each);
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, init) {
    LIU_FIND_LAST_MESSAGE;
    if(!message->hasAnInput()) return this;
    LIU_CHECK_INPUT_SIZE(1, 3);
    Text *text = Text::dynamicCast(message->runFirstInput());
    if(text) {
        LIU_CHECK_INPUT_SIZE(3);
        QString separator1 = message->runSecondInput()->toString();
        QString separator2 = message->runThirdInput()->toString();
        QStringList entries = text->value().split(separator1, QString::SkipEmptyParts);
        QStringListIterator i(entries);
        while (i.hasNext()) {
            QString entrie = i.next();
            int index = entrie.indexOf(separator2);
            if(index == -1) LIU_THROW_CONVERSION_EXCEPTION("key separator not found when building a Dictionary");
            set(Text::make(entrie.mid(0, index)), Text::make(entrie.mid(index + separator2.size())));
        }
        return this;
    }
    LIU_THROW_CONVERSION_EXCEPTION("cannot build a Dictionary with these arguments");
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, make) {
    LIU_FIND_LAST_MESSAGE;
    OldDictionary *dict = fork();
    for(int i = 0; i < message->numInputs(); ++i) {
        Node *key = NULL;
        Primitive *primitive = message->input(i)->label();
        if(!primitive) LIU_THROW(ArgumentException, "missing key in Dictionary initialization");
        if(primitive->hasNext()) LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
        Message *msg = Message::dynamicCast(primitive->value());
        if(msg) {
            if(msg->inputs(false) || msg->outputs(false) || msg->isEscaped() || msg->isParented()
                    || msg->isEllipsed() || msg->hasCodeInput())
                LIU_THROW(ArgumentException, "invalid key in Dictionary initialization");
            key = Text::make(msg->name());
        } else
            key = primitive->run();
        dict->set(key, message->runInput(i));
    }
    return dict;
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    Node *key = message->runFirstInput();
    if(message->isQuestioned() && !hasKey(key))
        return LIU_BOOLEAN(false);
    else
        return get(key);
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *key = message->runFirstInput();
    if(!hasKey(key)) LIU_THROW(NotFoundException, "key not found");
    return set(key, message->runSecondInput());
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, append_or_set) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Node *key = message->runFirstInput();
    return set(key, message->runSecondInput());
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, size) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_NUMBER(size());
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, empty) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    return LIU_BOOLEAN(isEmpty());
}

LIU_DEFINE_NATIVE_METHOD(OldDictionary, each) {
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->next();
    if(!nextPrimitive) LIU_THROW(InterpreterException, QString("missing code after an each statement"));
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    Message *msg = Message::dynamicCast(message->firstInput()->value()->value());
    if(!msg) LIU_THROW(ArgumentException, "first argument should be a message");
    QString valueName = msg->name();
    msg = Message::dynamicCast(message->secondInput()->value()->value());
    if(!msg) LIU_THROW(ArgumentException, "second argument should be a message");
    QString keyName = msg->name();
    Iterator i(this);
    while(i.hasNext()) {
        i.next();
        context()->addOrSetChild(keyName, i.key());
        context()->addOrSetChild(valueName, i.value());
        nextPrimitive->run();
    }
    Primitive::skip(this);
}
LIU_END
