#include "node/console.h"
#include "node/object/message.h"
#include "node/object/text.h"

LIU_BEGIN

LIU_DEFINE(Console, Node, Node);

void Console::initRoot() {
    LIU_ADD_NATIVE_METHOD(Console, write);
    LIU_ADD_NATIVE_METHOD(Console, print);
    LIU_ADD_NATIVE_METHOD(Console, alert);

    LIU_ADD_NATIVE_METHOD(Console, read);
}

LIU_DEFINE_NATIVE_METHOD(Console, write) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    QString str = message->runFirstInput()->toString();
    QTextStream(stdout) << str.toUtf8();
    return LIU_TEXT(str);
}

LIU_DEFINE_NATIVE_METHOD(Console, print) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    QString str = message->runFirstInput()->toString();
    LIU_MESSAGE("write", LIU_ARGUMENT_BUNCH(LIU_TEXT(str + "\n")))->run(this);
    return LIU_TEXT(str);
}

LIU_DEFINE_NATIVE_METHOD(Console, alert) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    QString str = message->runFirstInput()->toString();
    QTextStream(stderr) << (str + "\n").toUtf8();
    return LIU_TEXT(str);
}

LIU_DEFINE_NATIVE_METHOD(Console, read) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasAnInput())
        LIU_MESSAGE("write", message->inputs())->run(this);
    QString str;
    QTextStream(stdin) >> str;
    return LIU_TEXT(str);
}

LIU_END
