#include <QtCore/QFileInfo>

#include "node/exception.h"

LIU_BEGIN

LIU_EXCEPTION_DEFINITION(Exception, Node);

const QString Exception::report() const {
    QString str;
    if(!file.isEmpty()) str += QString("%1:").arg(QFileInfo(file).fileName());
    if(line != 0) str += QString("%1: ").arg(line);
    str += nodeName();
    if(!message.isEmpty()) str += QString(": %1").arg(message);
    if(!function.isEmpty()) str += QString(" in '%1'").arg(function);
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
