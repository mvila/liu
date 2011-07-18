#ifndef LIU_EXCEPTION_H
#define LIU_EXCEPTION_H

#include "node.h"
#include "node/runstack.h"

LIU_BEGIN

#define LIU_EXCEPTION(ARGS...) new Exception(context()->child("Exception"), ##ARGS)

#define LIU_THROW(EXCEPTION, MESSAGE) \
throw EXCEPTION(context()->child(#EXCEPTION), MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_TODO \
LIU_THROW(Exception, "function not yet implemented")

class Exception : public Node {
    LIU_DECLARE(Exception, Node, Node);
public:
    QString message;
    QString file;
    int line;
    QString function;
    RunStack *runStackCapture;

    explicit Exception(Node *origin, const QString &message = "", const QString &file = "",
              const int line = 0, const QString &function = "", RunStack *runStackCapture = runStack()->copy()) :
        Node(origin), message(message), file(file), line(line), function(function), runStackCapture(runStackCapture) {}

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Exception);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Exception, message, file, line, function, runStackCapture);

    const QString report() const;

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return report();
    }
};

#define LIU_EXCEPTION_DECLARATION(NAME, ORIGIN) \
class NAME : public ORIGIN { \
    LIU_DECLARE(NAME, ORIGIN, Node); \
public: \
    explicit NAME(Node *origin, const QString &message = "", const QString &file = "", \
         const int line = 0, const QString &function = "", RunStack *runStackCapture = runStack()->copy()) : \
        ORIGIN(origin, message, file, line, function, runStackCapture) {} \
    LIU_DECLARE_AND_DEFINE_COPY_METHOD(NAME); \
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(NAME, message, file, line, function, runStackCapture); \
};

#define LIU_EXCEPTION_DEFINITION(NAME, ORIGIN) \
LIU_DEFINE(NAME, ORIGIN, Node); \
void NAME::initRoot() {}

LIU_EXCEPTION_DECLARATION(LexerException, Exception);
LIU_EXCEPTION_DECLARATION(ParserException, Exception);
LIU_EXCEPTION_DECLARATION(RuntimeException, Exception);
LIU_EXCEPTION_DECLARATION(InterpreterException, RuntimeException);
LIU_EXCEPTION_DECLARATION(ArgumentException, RuntimeException);
LIU_EXCEPTION_DECLARATION(NullPointerException, RuntimeException);
LIU_EXCEPTION_DECLARATION(IndexOutOfBoundsException, RuntimeException);
LIU_EXCEPTION_DECLARATION(NotFoundException, RuntimeException);
LIU_EXCEPTION_DECLARATION(DuplicateException, RuntimeException);
LIU_EXCEPTION_DECLARATION(TypecastException, RuntimeException);
LIU_EXCEPTION_DECLARATION(ConversionException, RuntimeException);
LIU_EXCEPTION_DECLARATION(AssertionException, RuntimeException);
LIU_EXCEPTION_DECLARATION(OperatingSystemException, Exception);
LIU_EXCEPTION_DECLARATION(FileSystemException, OperatingSystemException);

LIU_END

#endif // LIU_EXCEPTION_H
