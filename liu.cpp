#define LIU_CATCH_EXCEPTIONS

#include <QtCore/QFile>

#include "liu.h"
#include "node/exception.h"
#include "node/object/message.h"
#include "node/object/application.h"
#include "node/object/language/primitive.h"
#include "node/object/language/interpreter.h"

LIU_BEGIN

void init() {
    foreach(Root root, roots()) root.node->setNodeName(root.name);
    #ifdef LIU_CATCH_EXCEPTIONS
    try {
    #endif
        Interpreter *interpreter = Interpreter::root();
        QString path = "../liu/";
        foreach(Root root, roots()) {
            Node *node = root.node;
            QString liuFile = path + node->nodePath() + node->nodeName().toLower() + ".liu";
            if(QFileInfo(liuFile).exists()) {
                interpreter->loadSourceCode(liuFile)->run();
            }
        }
        interpreter->testSuite()->run();
        P(QString("All tests passed (%1 sections, %2 assertions)").
          arg(interpreter->testSuite()->size()).arg(Node::passedAssertionCount()));

        // run Application
        pushContext(Application::root());
        SourceCode *source = interpreter->loadSourceCode("../liu/examples/test.liu");
        if(source->block()->isNotEmpty()) {
            P("-----------------------------------------------------------------------");
            source->inspect();
            P("-----------------------------------------------------------------------");
            P("-> " + source->run()->toString(true));
        }
        popContext();
    #ifdef LIU_CATCH_EXCEPTIONS
    } catch(Exception &e) {
        bool ok;
        Primitive *primitive = findLastPrimitive(e.runStackCapture, &ok);
        if(ok && !primitive->sourceCodeRef().isNull()) {
            const QString &source = *(primitive->sourceCodeRef().string());
            int column, line;
            computeColumnAndLineForPosition(source, primitive->sourceCodeRef().position(), column, line);
            QString text = extractLine(source, line);
            if(!text.isEmpty()) {
                QString cursor = QString(" ").repeated(column - 1).append("^");
                e.message += "\n" + text + "\n" + cursor;
                SourceCode *sourceCode = SourceCode::dynamicCast(primitive->findParentOriginatingFrom(SourceCode::root()));
                e.file = sourceCode ? sourceCode->url() : "";
                e.line = line;
                e.function.clear();
            }
        }
        QTextStream(stderr) << e.report().toUtf8() << '\n';
    }
    #endif
}

QList<Root> &roots() {
    static QList<Root> _roots;
    return _roots;
}

// === Miscellaneous ===

QString normalizeUrl(QString url) {
    if(!(url.isEmpty() || url.startsWith("file://") || url.startsWith("http://") || url.startsWith("liu:")))
        url.prepend("file://");
    if(url.startsWith("file://"))
        url = "file://" + QFileInfo(url.mid(7)).absoluteFilePath();
    return url;
}

QString readTextFile(const QString &name) {
    QFile file(name);
    if(!file.open(QIODevice::ReadOnly)) LIU_THROW(FileSystemException, "file not found");
    QString text = QTextStream(&file).readAll();
    file.close();
    return text;
}

QString concatenateStrings(const QString &first, const QString &separator, const QString &second) {
    QString str = first;
    if(!first.isEmpty() && !second.isEmpty()) str += separator;
    str += second;
    return str;
}

QString escapeTabsAndNewlines(QString text) {
    text.replace('\r', "\\r");
    text.replace('\n', "\\n");
    text.replace('\t', "\\t");
    return text;
}

QString extractLine(const QString &text, int requestedLine) {
    int line = 1;
    int pos = 0;
    while(pos != -1) {
        if(line == requestedLine) {
            int end = text.indexOf('\r', pos);
            if(end == -1) end = text.indexOf('\n', pos);
            if(end != -1)
                return text.mid(pos, end - pos);
            else
                return text.mid(pos) + ""; // Force non-null string
        }
        int pos2 = text.indexOf('\r', pos);
        if(pos2 != -1) {
            line++;
            pos = pos2 + 1;
            if(text.at(pos) == '\n') pos++; // Windows CR+LF case
        } else {
            pos = text.indexOf('\n', pos);
            if(pos != -1) {
                line++;
                pos++;
            }
        }
    }
    return QString();
}

bool computeColumnAndLineForPosition(const QString &text, const int position, int &column, int &line) {
    line = 1;
    int pos = 0;
    int previousPos = 0;
    bool crFound;
    while(true) {
        pos = text.indexOf('\r', previousPos);
        crFound = pos != -1;
        if(!crFound) pos = text.indexOf('\n', previousPos);
        if(pos == -1) pos = text.size();
        if(pos >= position) {
            column = position - previousPos + 1;
            return true;
        } else if(pos < text.size()) {
            line++;
            pos++;
            if(crFound && pos < text.size() && text.at(pos) == '\n') pos++; // Windows CR+LF case
            previousPos = pos;
        } else break;
    }
    return false;
}

QString preferSecondArgumentIfNotEmpty(const QString &a, const QString &b) {
    return b.isEmpty() ? a : b;
}

#define LIU_DEFINE_THROW_FUNCTION(EXCEPTION) \
void throw##EXCEPTION(const QString &message, const QString &file, const int line, const QString &function) { \
    throw EXCEPTION(context()->child(#EXCEPTION), message, file, line, function); \
}

LIU_DEFINE_THROW_FUNCTION(RuntimeException);
LIU_DEFINE_THROW_FUNCTION(NullPointerException);
LIU_DEFINE_THROW_FUNCTION(NotFoundException);
LIU_DEFINE_THROW_FUNCTION(TypecastException);
LIU_DEFINE_THROW_FUNCTION(ConversionException);

LIU_END
