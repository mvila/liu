#ifndef LIU_GLOBAL_H
#define LIU_GLOBAL_H

#include <QtCore/QPair>
#include <QtCore/QTextStream>
#include <QtCore/QStack>

//#define LIU_IS_NAMESPACED

#ifdef LIU_IS_NAMESPACED
#define LIU_BEGIN namespace Liu {
#define LIU_END }
#else
#define LIU_BEGIN
#define LIU_END
#endif

#ifdef LIU_IS_NAMESPACED
#define LIU_USE using namespace Liu;
#else
#define LIU_USE
#endif

LIU_BEGIN

#define P(VALUE) \
QTextStream(stdout) << VALUE << '\n';

#define PP P(Q_FUNC_INFO);

#define LIU_THROW_RUNTIME_EXCEPTION(MESSAGE) \
throwRuntimeException(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_THROW_NULL_POINTER_EXCEPTION(MESSAGE) \
throwNullPointerException(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_THROW_NOT_FOUND_EXCEPTION(MESSAGE) \
throwNotFoundException(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_THROW_TYPECAST_EXCEPTION(MESSAGE) \
throwTypecastException(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

#define LIU_THROW_CONVERSION_EXCEPTION(MESSAGE) \
throwConversionException(MESSAGE, __FILE__, __LINE__, Q_FUNC_INFO)

typedef long long int HugeInteger;
typedef unsigned long long int HugeUnsignedInteger;

typedef QPair<int, int> IntPair;

class Node;

void init();

// === roots ===

class Root {
public:
    Root(Node *node = NULL, const QString &name = "") : node(node), name(name) {}
    Node *node;
    QString name;
};

QList<Root> &roots();

// === contextStack ===

#define LIU_PUSH_CONTEXT(NODE) \
ContextPusher contextPusher(NODE); Q_UNUSED(contextPusher);

inline QStack<Node *> &contextStack() {
    static QStack<Node *> _stack;
    return _stack;
}

inline bool hasContext() {
    return !contextStack().isEmpty();
}

inline Node *context() {
    if(!hasContext()) qFatal("Fatal error: context stack is empty!");
    return contextStack().top();
}

inline void pushContext(Node *node) {
    contextStack().push(node);
}

inline Node *popContext() {
    if(!hasContext()) qFatal("Fatal error: context stack is empty!");
    return contextStack().pop();
}

class ContextPusher {
public:
    explicit ContextPusher(Node *node) { pushContext(node); }
    ~ContextPusher() { popContext(); }
private:
    ContextPusher(const ContextPusher &); // prevent copying
    ContextPusher &operator=(const ContextPusher &);
};

// === Miscellaneous ===

QString normalizeUrl(QString url);
QString readTextFile(const QString &name);
QString concatenateStrings(const QString &first, const QString &separator, const QString &second);
QString escapeTabsAndNewlines(QString text);
QString extractLine(const QString &text, int requestedLine);
bool computeColumnAndLineForPosition(const QString &text, const int position, int &column, int &line);
const QString &preferSecondArgumentIfNotEmpty(const QString &a, const QString &b);

#define LIU_DECLARE_THROW_FUNCTION(EXCEPTION) \
void throw##EXCEPTION(const QString &message = "", const QString &file = "", \
                        const int line = 0, const QString &function = "") __attribute__ ((noreturn));

LIU_DECLARE_THROW_FUNCTION(RuntimeException);
LIU_DECLARE_THROW_FUNCTION(NullPointerException);
LIU_DECLARE_THROW_FUNCTION(NotFoundException);
LIU_DECLARE_THROW_FUNCTION(TypecastException);
LIU_DECLARE_THROW_FUNCTION(ConversionException);

LIU_END

#endif // LIU_GLOBAL_H
