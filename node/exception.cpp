#include <QtCore/QFileInfo>

#include "node/exception.h"

LIU_BEGIN

LIU_DEFINE_2(Exception, Node, Node);

Exception *Exception::init(const QString *message, const QString *file, const int *line, const QString *function) {
    Node::init();
    setMessage(message);
    setFile(file);
    setLine(line);
    setFunction(function);
    setRunStackCapture(runStack()->copy());
    return this;
}

Exception *Exception::initCopy(const Exception *other) {
    Node::initCopy(other);
    setMessage(other->_message);
    setFile(other->_file);
    setLine(other->_line);
    setFunction(other->_function);
    if(_runStackCapture) setRunStackCapture(other->_runStackCapture->copy());
    return this;
}

Exception::~Exception() {
    setMessage();
    setFile();
    setLine();
    setFunction();
    deleteRunStackCapture(); // FIXME: dangerous, could be referenced elsewhere...
}

void Exception::initRoot() {}

LIU_DEFINE_ACCESSOR(Exception, QString, message, Message,);
LIU_DEFINE_ACCESSOR(Exception, QString, file, File,);
LIU_DEFINE_ACCESSOR(Exception, int, line, Line, 0);
LIU_DEFINE_ACCESSOR(Exception, QString, function, Function,);
LIU_DEFINE_NODE_ACCESSOR(Exception, RunStack, runStackCapture, RunStackCapture);
LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Exception, runStackCapture);

const QString Exception::report() const {
    QString str;
    if(!file().isEmpty()) str += QString("%1:").arg(QFileInfo(file()).fileName());
    if(line() != 0) str += QString("%1: ").arg(line());
    str += nodeName();
    if(!message().isEmpty()) str += QString(": %1").arg(message());
    if(!function().isEmpty()) str += QString(" in '%1'").arg(function());
    return str;
}

LIU_EXCEPTION_DEFINITION(LexerException, Exception);
LIU_EXCEPTION_DEFINITION(ParserException, Exception);
LIU_EXCEPTION_DEFINITION(RuntimeException, Exception);
LIU_EXCEPTION_DEFINITION(InterpreterException, RuntimeException);
LIU_EXCEPTION_DEFINITION(ArgumentException, RuntimeException);
LIU_EXCEPTION_DEFINITION(NullPointerException, RuntimeException);
LIU_EXCEPTION_DEFINITION(IndexOutOfBoundsException, RuntimeException);
LIU_EXCEPTION_DEFINITION(NotFoundException, RuntimeException);
LIU_EXCEPTION_DEFINITION(DuplicateException, RuntimeException);
LIU_EXCEPTION_DEFINITION(TypecastException, RuntimeException);
LIU_EXCEPTION_DEFINITION(ConversionException, RuntimeException);
LIU_EXCEPTION_DEFINITION(AssertionException, RuntimeException);
LIU_EXCEPTION_DEFINITION(MathException, RuntimeException);
LIU_EXCEPTION_DEFINITION(OperatingSystemException, Exception);
LIU_EXCEPTION_DEFINITION(FileSystemException, OperatingSystemException);

LIU_END
