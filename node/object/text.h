#ifndef LIU_TEXT_H
#define LIU_TEXT_H

#include "node/object/element.h"
#include "node/object/iterable.h"
#include "node/object/boolean.h"
#include "node/object/number.h"
#include "node/object/language/message.h"

LIU_BEGIN

#define LIU_TEXT(ARGS...) new Text(context()->child("Object", "Text"), ##ARGS)

class Text : public Iterable {
    LIU_DECLARE(Text, Object, Object);
public:
    explicit Text(Node *origin, const QString &value = "", bool isTranslatable = false,
                  QList<IntPair> *interpolableSlices = NULL);

    Text(const Text &other);

    virtual ~Text() { delete _interpolableSlices; }

    LIU_DECLARE_AND_DEFINE_COPY_METHOD(Text);
    LIU_DECLARE_AND_DEFINE_FORK_METHOD(Text, value(), isTranslatable(), interpolableSlices());

    LIU_DECLARE_NATIVE_METHOD(init);

    QString value() const { return _value; }

    virtual void setValue(const QString &newValue) {
        if(newValue != value()) {
            _value = newValue;
            hasChanged();
        }
    }

    virtual void hasChanged() {}

    bool isTranslatable() const { return _isTranslatable; }
    void setIsTranslatable(bool isTranslatable) { _isTranslatable = isTranslatable; }

    QList<IntPair> *interpolableSlices() const { return _interpolableSlices; }

    void setInterpolableSlices(QList<IntPair> *interpolableSlices) {
        _interpolableSlices = interpolableSlices && !interpolableSlices->isEmpty() ?
                    new QList<IntPair>(*interpolableSlices) : NULL;
    }

    virtual Node *run(Node *receiver = context());

    LIU_DECLARE_NATIVE_METHOD(get);

    LIU_DECLARE_NATIVE_METHOD(concatenate);
    LIU_DECLARE_NATIVE_METHOD(multiply);

    LIU_DECLARE_NATIVE_METHOD(uppercase);
    LIU_DECLARE_NATIVE_METHOD(lowercase);

    static QString capitalize(QString text);
    LIU_DECLARE_NATIVE_METHOD(capitalize);

    LIU_DECLARE_NATIVE_METHOD(extract_between);
    LIU_DECLARE_NATIVE_METHOD(remove_after);
    LIU_DECLARE_NATIVE_METHOD(replace);

    virtual bool contains(Node *what) const;
    virtual int count(Node *what) const;
    virtual int size() const;
    virtual bool empty() const;

    virtual Iterator *iterator() const;

    virtual bool isEqualTo(const Node *other) const;
    LIU_DECLARE_NATIVE_METHOD(equal_to);

    virtual short compare(const Node *other) const;
    short compare(const QString &other) const;
    LIU_DECLARE_NATIVE_METHOD(compare);

    static QString unescapeSequence(const QString &source, QList<IntPair> *interpolableSlices = NULL);
    static QChar unescapeSequenceNumber(const QString &source, int &i);

    virtual uint hash() const { return qHash(_value); }

    virtual double toDouble(bool *okPtr = NULL) const;
    virtual QChar toChar() const;
    virtual QString toString(bool debug = false, short level = 0) const;
private:
    QString _value;
    bool _isTranslatable;
    QList<IntPair> *_interpolableSlices;
public:
    // === Iterator ===

    #define LIU_TEXT_ITERATOR(ARGS...) new Iterator(context()->child("Object", "Text", "Iterator"), ##ARGS)

    class Iterator : public Iterable::Iterator {
        LIU_DECLARE(Iterator, Object, Text);
    public:
        explicit Iterator(Node *origin, Text *text = NULL);

        virtual ~Iterator() {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator);

        LIU_DECLARE_NATIVE_METHOD(init);

        virtual bool hasNext() const;
        virtual Text *peekNext() const;
        virtual void skipNext();
    private:
        Text *_text;
        int _index;
    };
/*
    #define LIU_TEXT_ITERATOR(ARGS...) new Iterator(context()->child("Object", "Text", "Iterator"), ##ARGS)

    #define LIU_CHECK_TEXT_ITERATOR_INDEX \
    if(!hasValue()) LIU_THROW(IndexOutOfBoundsException, "Iterator is out of bounds");

    class Iterator : public Object {
        LIU_DECLARE(Iterator, Object, Text);
    public:
        explicit Iterator(Node *origin, Text *text = NULL, int index = 0) :
            Object(origin), _text(text), _index(index) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Iterator);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Iterator, _text, _index);

        LIU_DECLARE_NATIVE_METHOD(init);

        QChar value() const {
            LIU_CHECK_TEXT_ITERATOR_INDEX;
            return _text->value().at(_index);
        }

        LIU_DECLARE_NATIVE_METHOD(value);

        bool hasValue() const {
            if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
            return _index >= 0 && _index < _text->value().size();
        }

        void first() { _index = 0; }
        LIU_DECLARE_NATIVE_METHOD(first);

        void last() {
            if(!_text) LIU_THROW(NullPointerException, "Text pointer is NULL");
            _index = _text->value().size() - 1;
        }

        LIU_DECLARE_NATIVE_METHOD(last);

        void next() { LIU_CHECK_TEXT_ITERATOR_INDEX; _index++; }
        LIU_DECLARE_NATIVE_METHOD(prefix_increment);

        void previous() { LIU_CHECK_TEXT_ITERATOR_INDEX; _index--; }
        LIU_DECLARE_NATIVE_METHOD(prefix_decrement);
    private:
        Text *_text;
        int _index;
    };
*/
};

LIU_END

#endif // LIU_TEXT_H
