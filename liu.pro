#-------------------------------------------------
#
# Project created by QtCreator 2010-08-23T15:21:38
#
#-------------------------------------------------

QT       += core
QT       += testlib
QT       -= gui

TARGET = liu
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    liu.cpp \
    node.cpp \
    node/exception.cpp \
    node/alias.cpp \
    node/nativemethod.cpp \
    node/runstack.cpp \
    node/controlflow.cpp \
    node/console.cpp \
    node/object.cpp \
    node/object/property.cpp \
    node/object/element.cpp \
    node/object/boolean.cpp \
    node/object/number.cpp \
    node/object/character.cpp \
    node/object/text.cpp \
    node/object/pair.cpp \
    node/object/list.cpp \
    node/object/bunch.cpp \
    node/object/dictionary.cpp \
    node/object/method.cpp \
    node/object/message.cpp \
    node/object/application.cpp \
    node/object/language.cpp \
    node/object/language/token.cpp \
    node/object/language/operator.cpp \
    node/object/language/operatortable.cpp \
    node/object/language/primitive.cpp \
    node/object/language/argument.cpp \
    node/object/language/argumentbunch.cpp \
    node/object/language/parameter.cpp \
    node/object/language/parameterlist.cpp \
    node/object/language/section.cpp \
    node/object/language/block.cpp \
    node/object/language/sourcecode.cpp \
    node/object/language/sourcecodedictionary.cpp \
    node/object/language/test.cpp \
    node/object/language/testsuite.cpp \
    node/object/language/lexer.cpp \
    node/object/language/parser.cpp \
    node/object/language/interpreter.cpp \
    tests/nodetest.cpp \
    tests/objecttest.cpp \
    tests/texttest.cpp \
    tests/listtest.cpp \
    tests/dictionarytest.cpp

HEADERS += \
    liu.h \
    node.h \
    node/exception.h \
    node/alias.h \
    node/nativemethod.h \
    node/runstack.h \
    node/controlflow.h \
    node/console.h \
    node/object.h \
    node/object/property.h \
    node/object/element.h \
    node/object/boolean.h \
    node/object/number.h \
    node/object/character.h \
    node/object/text.h \
    node/object/pair.h \
    node/object/list.h \
    node/object/bunch.h \
    node/object/dictionary.h \
    node/object/method.h \
    node/object/message.h \
    node/object/application.h \
    node/object/language.h \
    node/object/language/token.h \
    node/object/language/operator.h \
    node/object/language/operatortable.h \
    node/object/language/primitive.h \
    node/object/language/argument.h \
    node/object/language/argumentbunch.h \
    node/object/language/parameter.h \
    node/object/language/parameterlist.h \
    node/object/language/section.h \
    node/object/language/block.h \
    node/object/language/sourcecode.h \
    node/object/language/sourcecodedictionary.h \
    node/object/language/test.h \
    node/object/language/testsuite.h \
    node/object/language/lexer.h \
    node/object/language/parser.h \
    node/object/language/interpreter.h \
    tests/testbegin.h \
    tests/testend.h \
    tests/runalltests.h \
    tests/nodetest.h \
    tests/objecttest.h \
    tests/texttest.h \
    tests/listtest.h \
    tests/dictionarytest.h

OTHER_FILES += \
    README.txt \
    notes/ideas.txt \
    notes/syntax.txt \
    notes/pseudocode.txt \
    notes/todo.txt \
    notes/old.txt \
    notes/specification.txt \
    notes/features.txt \
    examples/test.liu \
    node.liu \
    node/exception.liu \
    node/object.liu \
    node/object/text.liu \
    node/object/abstractlist.liu \
    misc/kate/liu.xml \
    examples/console_read.liu
