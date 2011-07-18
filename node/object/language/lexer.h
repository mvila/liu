#ifndef LIU_LANGUAGE_LEXER_H
#define LIU_LANGUAGE_LEXER_H

#include <QtCore/QFileInfo>

#include "node/object/language/token.h"
#include "node/object/language/operatortable.h"

LIU_BEGIN

namespace Language {
    #define LIU_LEXER(ARGS...) \
    new Language::Lexer(context()->child("Object", "Language", "Lexer"), ##ARGS)

    class Lexer : public Object {
        LIU_DECLARE(Lexer, Object, Language);
    public:
        explicit Lexer(Node *origin) : Object(origin), _operatorTable(NULL), _source(NULL) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Lexer);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Lexer); // TODO

        OperatorTable *operatorTable() const;

        void setSource(const QString &source) { _source = &source; rewind(); }
        const QString &source() const { return *_source; }

        void setResourceUrl(const QString &name) { _resourceUrl = name;}
        const QString resourceUrl() const { return _resourceUrl; }
        const QString resourceName() const { return QFileInfo(resourceUrl()).fileName(); }

        void rewind();
        Token *nextToken();

        void consume();

        void startToken() { _tokenPosition = _position; }
        Token *finishToken(const Token::Type type) { return LIU_TOKEN(type, tokenTextRef()); }

        const QStringRef tokenTextRef() const {
            return source().midRef(_tokenPosition, _position - _tokenPosition);
        }

        Token *scan(const Token::Type type) { // Simple one char tokens
            startToken();
            consume();
            return finishToken(type);
        }

        bool isEof() const { return _currentChar.isNull(); }

        bool isSpace() const { return _currentChar == ' ' || _currentChar == '\t'; }
        void consumeSpaces() { do consume(); while(isSpace()); }

        bool isNewline() const { return _currentChar == '\r' || _currentChar == '\n'; }
        Token *scanNewline();

        bool isLineComment() const { return _currentChar == '/' && _nextChar == '/'; }
        void consumeLineComment();

        bool isBlockComment() const { return _currentChar == '/' && _nextChar == '*'; }
        void consumeBlockComment();

        bool isName() const { return _currentChar.isLetter() || _currentChar == '_'; }
        Token *scanName();

        bool isBackquotedName() const { return _currentChar == '`'; }
        Token *scanBackquotedName();

        bool isOperator() const { return operatorTable()->hasOperatorStartingWith(_currentChar); }
        Token *scanOperator();

        bool isNumber() const { return _currentChar.isNumber(); }
        Token *scanNumber();

        Token *scanCharacter();
        Token *scanText();

        void consumeEscapeSequence();
        void consumeEscapeSequenceNumber();

        Token *scanRightBracket();

        LexerException lexerException(QString message) const;

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        OperatorTable *_operatorTable;
        const QString *_source;
        QString _resourceUrl;
        int _position;
        QChar _previousChar;
        QChar _currentChar;
        QChar _nextChar;
        int _tokenPosition;
    };
}

LIU_END

#endif // LIU_LANGUAGE_LEXER_H
