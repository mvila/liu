#include "node/object/text.h"
#include "node/object/language/interpreter.h"
#include "node/object/language/nativemethod.h"

LIU_BEGIN

// === Text ===

LIU_DEFINE(Text, Object, Object);

Text::Text(Node *origin, QString *value, bool *isTranslatable, QList<IntPair> *interpolableSlices) :
    Iterable(origin), _value(value), _isTranslatable(isTranslatable), _interpolableSlices(interpolableSlices),
    _hasValue(false), _hasIsTranslatable(false), _hasInterpolableSlices(false) {
    Text *orig = Text::dynamicCast(origin);
    if(orig) {
        _value = orig->_value;
        _isTranslatable = orig->_isTranslatable;
        _interpolableSlices = orig->_interpolableSlices;
    }
}

Text::Text(Node *origin, const QString &value, bool isTranslatable, QList<IntPair> *interpolableSlices) :
    Iterable(origin), _value(NULL), _isTranslatable(NULL), _interpolableSlices(NULL),
    _hasValue(false), _hasIsTranslatable(false), _hasInterpolableSlices(false) {
    setValue(value);
    setIsTranslatable(isTranslatable);
    setInterpolableSlices(interpolableSlices);
}

Text::Text(const Text &other) : Iterable(other), _value(NULL), _isTranslatable(NULL), _interpolableSlices(NULL),
    _hasValue(false), _hasIsTranslatable(false), _hasInterpolableSlices(false) {
    setValue(other.value());
    setIsTranslatable(other.isTranslatable());
    setInterpolableSlices(other.interpolableSlices());
}

Text::~Text() {
    if(_hasValue) delete _value;
    if(_hasIsTranslatable) delete _isTranslatable;
    if(_hasInterpolableSlices) delete _interpolableSlices;
}

void Text::initRoot() {
    addExtension(Iterable::root());

    LIU_ADD_NATIVE_METHOD(Text, init);

    LIU_ADD_NATIVE_METHOD(Text, get, []);

    LIU_ADD_NATIVE_METHOD(Text, concatenate, +);
    LIU_ADD_NATIVE_METHOD(Text, multiply, *);

    LIU_ADD_NATIVE_METHOD(Text, uppercase);
    LIU_ADD_NATIVE_METHOD(Text, lowercase);
    LIU_ADD_NATIVE_METHOD(Text, capitalize);

    LIU_ADD_NATIVE_METHOD(Text, extract_between);
    LIU_ADD_NATIVE_METHOD(Text, remove_after);
    LIU_ADD_NATIVE_METHOD(Text, replace);

    LIU_ADD_NATIVE_METHOD(Text, equal_to, ==);
    LIU_ADD_NATIVE_METHOD(Text, compare, <=>);
}

void Text::initFork() {
}

LIU_DEFINE_NATIVE_METHOD(Text, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasAnInput()) setValue(message->runFirstInput()->toString());

    // === TODO: DRY ===
    LIU_FIND_LAST_PRIMITIVE;
    Primitive *nextPrimitive = primitive->next();
    if(nextPrimitive) {
        nextPrimitive->run(this);
        Primitive::skip(this);
    }

    return this;
}

const QString &Text::value() const {
    return *_value;
}

void Text::setValue(const QString &newValue) {
    if(_hasValue)
        *_value = newValue;
    else {
        _value = new QString(newValue);
        _hasValue = true;
    }
    hasChanged();
}

bool Text::isTranslatable() const {
    return *_isTranslatable;
}

void Text::setIsTranslatable(bool isTranslatable) {
    if(_hasIsTranslatable)
        *_isTranslatable = isTranslatable;
    else {
        _isTranslatable = new bool(isTranslatable);
        _hasIsTranslatable = true;
    }
    hasChanged();
}

QList<IntPair> *Text::interpolableSlices() const {
    return _interpolableSlices;
}

// A revoir : modifier la macro LIU_TEXT pour faire systématiquement des forks du root et ainsi récupérer un pointeur
// vers un interpolableSlices vide
void Text::setInterpolableSlices(QList<IntPair> *interpolableSlices) {
    if(_hasInterpolableSlices) {
        if(interpolableSlices)
            *_interpolableSlices = *interpolableSlices;
        else
            _interpolableSlices->clear();
    } else {
        _interpolableSlices = interpolableSlices ? new QList<IntPair>(*interpolableSlices) : new QList<IntPair>;
        _hasInterpolableSlices = true;
    }
    hasChanged();
}

Node *Text::run(Node *receiver) {
    Q_UNUSED(receiver);
    if(interpolableSlices()) {
        LIU_PUSH_RUN(this);
        QString result = value();
        QString source;
        QString str;
        int offset = 0;
        foreach (IntPair slice, *interpolableSlices()) {
            source = result.mid(offset + slice.first + 1, slice.second - 2);
            str = !source.isEmpty() ? Interpreter::root()->runSourceCode("liu:" + source)->toString() : "";
            result.replace(offset + slice.first, slice.second, str);
            offset += str.size() - slice.second;
        }
        return LIU_TEXT(result);
    } else
        return this;
}

LIU_DEFINE_NATIVE_METHOD(Text, get) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1, 2);
    int index = message->runFirstInput()->toDouble();
    int max = value().size();
    if(index < 0) index = max + index;
    if(index < 0 || index >= max) LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    int size = message->hasASecondInput() ? message->runSecondInput()->toDouble() : 1;
    if(size < 0) size = max + (size + 1) - index;
    if(size < 0 || index + size > max) LIU_THROW(IndexOutOfBoundsException, "index is out of bounds");
    return LIU_TEXT(value().mid(index, size));
}

LIU_DEFINE_NATIVE_METHOD(Text, concatenate) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_TEXT(value() + message->runFirstInput()->toString());
}

LIU_DEFINE_NATIVE_METHOD(Text, multiply) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_TEXT(value().repeated(message->runFirstInput()->toDouble()));
}

LIU_DEFINE_NATIVE_METHOD(Text, uppercase) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return LIU_TEXT(value().toUpper());
    else {
        setValue(value().toUpper());
        return this;
    }
}

LIU_DEFINE_NATIVE_METHOD(Text, lowercase) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return LIU_TEXT(value().toLower());
    else {
        setValue(value().toLower());
        return this;
    }
}

QString Text::capitalize(QString text) {
    if(!text.isEmpty()) text[0] = text[0].toUpper();
    return text;
}

LIU_DEFINE_NATIVE_METHOD(Text, capitalize) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isExclaimed())
        return LIU_TEXT(capitalize(value()));
    else {
        setValue(capitalize(value()));
        return this;
    }
}

LIU_DEFINE_NATIVE_METHOD(Text, extract_between) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    QString text = value();
    QString after = message->runFirstInput()->toString();
    QString before = message->runSecondInput()->toString();
    int from = 0;
    if(!after.isEmpty()) {
        from = text.indexOf(after);
        if(from == -1) return LIU_BOOLEAN(false);
    }
    int to;
    if(!before.isEmpty()) {
        to = text.indexOf(before, from);
        if(to == -1) return LIU_BOOLEAN(false);
        to += before.size() - 1;
    } else {
        to = text.size() - 1;
    }
    Node *result = LIU_TEXT(text.mid(from + after.size(),
                         (to - before.size()) - (from + after.size()) + 1));
    if(message->isExclaimed())
        setValue(text.remove(from, to - from + 1));
    return result;
}

LIU_DEFINE_NATIVE_METHOD(Text, remove_after) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    QString text = value();
    QString after = message->runFirstInput()->toString();
    if(!after.isEmpty()) {
        int index = text.indexOf(after);
        if(index != -1) text.remove(index, text.size() - index);
    }
    if(message->isExclaimed()) {
        setValue(text);
        return this;
    } else
        return LIU_TEXT(text);
}

LIU_DEFINE_NATIVE_METHOD(Text, replace) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(2);
    QString oldText = message->runFirstInput()->toString();
    if(oldText.isEmpty()) LIU_THROW(ArgumentException, "first argument is empty");
    QString newText = message->runSecondInput()->toString();
    QString source = value();
    source.replace(oldText, newText);
    if(message->isExclaimed()) {
        setValue(source);
        return this;
    } else
        return LIU_TEXT(source);
}

bool Text::contains(Node *what) const {
    return value().contains(what->toString());
}

int Text::count(Node *what) const {
    return value().count(what->toString());
}

int Text::size() const {
    return value().size();
}

bool Text::empty() const {
    return value().isEmpty();
}

Iterable::Iterator *Text::iterator() const {
    return LIU_TEXT_ITERATOR(constCast(this));
}

bool Text::isEqualTo(const Node *other) const {
    const Text *otherText = Text::dynamicCast(other);
    return otherText && value() == otherText->value();
}

LIU_DEFINE_NATIVE_METHOD(Text, equal_to) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_BOOLEAN(value() == message->runFirstInput()->toString());
}

short Text::compare(const Node *other) const {
    return compare(Text::cast(other)->value());
}

short Text::compare(const QString &other) const {
    int result = value().compare(other);
    if(result > 0) return 1;
    else if(result < 0) return -1;
    else return 0;
}

LIU_DEFINE_NATIVE_METHOD(Text, compare) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(1);
    return LIU_NUMBER(compare(message->runFirstInput()->toString()));
}

QString Text::unescapeSequence(const QString &source, QList<IntPair> *interpolableSlices) {
    QString result;
    int i = 0;
    QChar c;
    while(i < source.size()) {
        c = source.at(i);
        if(c == '{' && interpolableSlices) {
            int end = source.indexOf('}', i + 1);
            if(end == -1) LIU_THROW(RuntimeException, "'}' not found after '{'");
            interpolableSlices->append(IntPair(result.size(), end - i + 1));
            result += source.mid(i, end - i + 1);
            i = end + 1;
        } else {
            if(c == '\\') {
                i++;
                if(i == source.size()) LIU_THROW(RuntimeException, "invalid escape sequence: '\\'");
                c = source.at(i);
                switch(c.toAscii()) {
                case 't': c = '\t';  break;
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case '"': c = '"'; break;
                case '\'': c = '\''; break;
                case '{': c = '{'; break;
                case '\\': c = '\\'; break;
                default:
                    if(QString("01234567xu").contains(c, Qt::CaseInsensitive))
                        c = Text::unescapeSequenceNumber(source, i);
                    else
                        LIU_THROW(RuntimeException, QString("unknown escape sequence: '\\%1'").arg(c));
                }
            }
            result += c;
            i++;
        }
    }
    return result;
}

QChar Text::unescapeSequenceNumber(const QString &source, int &i) {
    char type;
    QString allowedChars;
    short maxSize;
    QChar c = source.at(i);
    if(c == 'x' || c == 'X') {
        type = 'x';
        allowedChars = "0123456789abcdef";
        maxSize = 2;
    } else if(c == 'u' || c == 'U') {
        type = 'u';
        allowedChars = "0123456789abcdef";
        maxSize = 4;
    } else {
        type = 'o';
        allowedChars = "01234567";
        maxSize = 3;
        i--;
    }
    QString number = "";
    do {
        i++;
        if(i == source.size()) LIU_THROW(RuntimeException, "invalid escape sequence number");
        c = source.at(i);
        if(!allowedChars.contains(c, Qt::CaseInsensitive)) { i--; break; }
        number += c;
    } while(number.size() < maxSize);
    if(number.isEmpty()) LIU_THROW(RuntimeException, "invalid escape sequence number");
    bool ok;
    ushort code = type == 'o' ? number.toUShort(&ok, 8) : number.toUShort(&ok, 16);
    if(!ok || (type != 'u' && code > 0xFF)) LIU_THROW(RuntimeException, "invalid number in escape sequence");
    return QChar(code);
}

double Text::toDouble(bool *okPtr) const {
    bool ok;
    double number = value().toDouble(&ok);
    if(okPtr)
        *okPtr = ok;
    else if(!ok)
        LIU_THROW_CONVERSION_EXCEPTION("conversion from Text to Number failed");
    return ok ? number : 0;
};

QChar Text::toChar() const {
    if(value().size() != 1) LIU_THROW_CONVERSION_EXCEPTION("conversion from Text to Character failed (size should be equal to 1)");
    return value().at(0);
};

QString Text::toString(bool debug, short level) const {
    Q_UNUSED(level);
    return debug ? "\"" + value() + "\"" : value();
}

// === Text::Iterator ===

LIU_DEFINE(Text::Iterator, Iterable::Iterator, Text);

Text::Iterator::Iterator(Node *origin, Text *text) :
    Iterable::Iterator(origin), _text(text), _index(0) {}

void Text::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Text::Iterator, init);
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0, 1);
    if(message->hasAnInput()) {
        Text *text = Text::dynamicCast(message->runFirstInput());
        if(!text) LIU_THROW(ArgumentException, "text argument is expected");
        _text = text;
    }
    return this;
}

bool Text::Iterator::hasNext() const {
    if(!_text) return false;
    return _index < _text->value().size() ;
}

Text *Text::Iterator::peekNext() const {
    if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
    if(_index >= _text->value().size()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    return LIU_TEXT(_text->value().at(_index));
}

void Text::Iterator::skipNext() {
    if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
    if(_index >= _text->value().size()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");
    _index++;
}

/*
// === Text::Iterator ===

LIU_DEFINE(Text::Iterator, Object, Text);

void Text::Iterator::initRoot() {
    LIU_ADD_NATIVE_METHOD(Text::Iterator, init);

    LIU_ADD_NATIVE_METHOD(Text::Iterator, value);

    LIU_ADD_NATIVE_METHOD(Text::Iterator, first);
    LIU_ADD_NATIVE_METHOD(Text::Iterator, last);

    LIU_ADD_NATIVE_METHOD(Text::Iterator, prefix_increment, prefix++);
    LIU_ADD_NATIVE_METHOD(Text::Iterator, prefix_decrement, prefix--);
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, init) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    _text = Text::dynamicCast(origin()->parent());
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, value) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    if(!message->isQuestioned())
        return LIU_TEXT(value());
    else
        return LIU_BOOLEAN(hasValue());
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, first) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_EXCLAMATION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    first();
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, last) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_EXCLAMATION_MARK;
    LIU_CHECK_INPUT_SIZE(0);
    last();
    return this;
}

LIU_DEFINE_NATIVE_METHOD(Text::Iterator, prefix_increment) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    next();
    return this;
}


LIU_DEFINE_NATIVE_METHOD(Text::Iterator, prefix_decrement) {
    LIU_FIND_LAST_MESSAGE;
    LIU_CHECK_INPUT_SIZE(0);
    previous();
    return this;
}
*/

LIU_END
