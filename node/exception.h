#ifndef LIU_EXCEPTION_H
#define LIU_EXCEPTION_H

#include "node.h"
#include "node/runstack.h"

LIU_BEGIN

#define LIU_THROW(EXCEPTION, MESSAGE) \
throw EXCEPTION::make(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_TODO \
LIU_THROW(Exception, "function not yet implemented")

#define LIU_ABSTRACT_CALL \
LIU_THROW(RuntimeException, "abstract method called");

class Exception : public Node {
    LIU_DECLARE_2(Exception, Node, Node);
public:
    explicit Exception(Node *origin = context()->child("Exception")) :
        Node(origin), _message(NULL), _file(NULL), _line(NULL), _function(NULL), _runStackCapture(NULL) {}

    static Exception *make(const QString &message) { return (new Exception())->init(&message); }
    static Exception *make(const QString &message, const QString &file) { return (new Exception())->init(&message, &file); }
    static Exception *make(const QString &message, const QString &file, const int &line) {
        return (new Exception())->init(&message, &file, &line); }
    static Exception *make(const QString &message, const QString &file, const int &line, const QString &function) {
        return (new Exception())->init(&message, &file, &line, &function); }

    Exception *init(const QString *message = NULL, const QString *file = NULL, const int *line = NULL,
                    const QString *function = NULL);

    LIU_DECLARE_ACCESSOR(QString, message, Message);
    LIU_DECLARE_ACCESSOR(QString, file, File);
    LIU_DECLARE_ACCESSOR(int, line, Line);
    LIU_DECLARE_ACCESSOR(QString, function, Function);
    LIU_DECLARE_NODE_ACCESSOR(RunStack, runStackCapture, RunStackCapture);

    const QString report() const;

    virtual QString toString(bool debug = false, short level = 0) const {
        Q_UNUSED(debug);
        Q_UNUSED(level);
        return report();
    }
private:
    QString *_message;
    QString *_file;
    int *_line;
    QString *_function;
    RunStack *_runStackCapture;
};

#define LIU_EXCEPTION_DECLARATION(NAME, ORIGIN) \
class NAME : public ORIGIN { \
    LIU_DECLARE_2(NAME, ORIGIN, Node); \
public: \
    explicit NAME(Node *origin = context()->child(#NAME)) : ORIGIN(origin) {} \
    static NAME *make(const QString &message) { return (new NAME())->init(&message); } \
    static NAME *make(const QString &message, const QString &file) { return (new NAME())->init(&message, &file); } \
    static NAME *make(const QString &message, const QString &file, const int &line) { \
        return (new NAME())->init(&message, &file, &line); } \
    static NAME *make(const QString &message, const QString &file, const int &line, const QString &function) { \
        return (new NAME())->init(&message, &file, &line, &function); } \
    NAME *init(const QString *message = NULL, const QString *file = NULL, const int *line = NULL, \
                    const QString *function = NULL); \
};

#define LIU_EXCEPTION_DEFINITION(NAME, ORIGIN) \
LIU_DEFINE_2(NAME, ORIGIN, Node); \
\
NAME *NAME::init(const QString *message, const QString *file, const int *line, const QString *function) { \
    ORIGIN::init(message, file, line, function); \
    return this; \
} \
\
NAME *NAME::initCopy(const NAME *other) { \
    ORIGIN::initCopy(other); \
    return this; \
} \
\
NAME::~NAME() {} \
\
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
LIU_EXCEPTION_DECLARATION(MathException, RuntimeException);
LIU_EXCEPTION_DECLARATION(OperatingSystemException, Exception);
LIU_EXCEPTION_DECLARATION(FileSystemException, OperatingSystemException);

LIU_END

#endif // LIU_EXCEPTION_H
