#include "node/object/language/parser.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Parser, Object, Language);

    void Parser::initRoot() {
    }

    Lexer *Parser::lexer() const {
        if(!_lexer)
            constCast(this)->_lexer = Lexer::cast(constCast(this)->child("lexer")); // search a Lexer in parents
        return _lexer;
    }

    Block *Parser::parse(const QString &source, const QString &resourceUrl) {
        lexer()->setSource(source);
        lexer()->setResourceUrl(resourceUrl);
        clearOpenedTokens();
        consume();
        return scanBlock();
    }

    bool Parser::is(const Token::Type type, const QString &text) const {
        if(text.isNull())
            return tokenType() == type;
        else
            return tokenType() == type && tokenText() == text;
    }

    const QString Parser::match(const Token::Type type, const QString &text) {
        if(is(type, text)) {
            QString text(tokenText());
            consume();
            consumeUselessNewline();
            return text;
        } else {
            const QString optionalText = text.isNull() ? "" : "'" + text + "' ";
            throw parserException("expecting " + optionalText + Token::typeName(type) + ", found " + tokenTypeName());
        }
    }

    void Parser::consumeNewline() {
        while(is(Token::Newline) || is(Token::Semicolon))
            consume();
    }

    void Parser::consumeUselessNewline() {
        if(!is(Token::Newline)) return;
        Token::Type type = topToken();
        if(type == Token::LeftParenthesis || type == Token::LeftBracket)
           do consume(); while(is(Token::Newline));
    }

    Block *Parser::scanBlock(const Token::Type terminator) {
        Block *block = LIU_BLOCK();
        Section *section = NULL;
        consumeNewline();
        while(!is(terminator)) {
            Primitive *expression = scanExpression();
            if(expression)
                if(Pair *pair = Pair::dynamicCast(expression->value())) {
                    section = LIU_SECTION();
                    section->setLabel(Primitive::cast(pair->first()));
                    expression = Primitive::cast(pair->second());
                    block->append(section);
                }
            if(expression) {
                if(!section) {
                    section = LIU_SECTION();
                    block->append(section);
                }
                section->append(expression);
            }
            consumeNewline();
        }
        return block;
    }

    Primitive *Parser::scanExpression() {
        Primitive *primitive = scanUnaryExpressionChain();
        if(!primitive) throw parserException("expecting UnaryExpression, found " + tokenTypeName());
        if(Operator *op = isBinaryOperator())
            return scanBinaryOperator(primitive, op, 0);
        return primitive;
    }

    Primitive *Parser::scanUnaryExpressionChain() {
        Primitive *primitive = NULL;
        while(isUnaryExpression()) {
            primitive = scanUnaryExpression(primitive);
            if(isBinaryOperator()) break;
        }
        return primitive;
    }

    Primitive *Parser::scanUnaryExpression(Primitive *currentPrimitive) {
        if(Operator *op = isPrefixOperator())
            return scanPrefixOperator(currentPrimitive, op);
        else
            return scanPrimaryExpression(currentPrimitive);
    }

    Primitive *Parser::scanPrimaryExpression(Primitive *currentPrimitive) {
        LIU_PRIMITIVE_ADD(currentPrimitive, scanOperand());
        while(isCollectionAccessor())
            currentPrimitive = scanCollectionAccessor(currentPrimitive);
        while(Operator *op = isPostfixOperator())
            currentPrimitive = scanPostfixOperator(currentPrimitive, op);
        return currentPrimitive;
    }

    Primitive *Parser::scanOperand() {
        if(isName()) {
            return scanName();
        } else if(isLiteral()) {
            return scanLiteral();
        } else if(isCollection()) {
            return scanCollection();
        } else if(isSubexpression()) {
            return scanSubexpression();
        } else if(isNestedBlock()) {
            return scanNestedBlock();
        }
        throw parserException("unimplemented operand");
    }

    Primitive *Parser::scanName() {
        Message *message = LIU_MESSAGE();
        QString name = tokenText();
        if(name.endsWith('?') && name.size() > 1) {
            message->setIsQuestioned(true);
            name.remove('?');
        }
        if(name.endsWith('!') && name.size() > 1) {
            message->setIsExclaimed(true);
            name.remove('!');
        }
        message->setName(name);
        Primitive *primitive = LIU_PRIMITIVE(message);
        int begin = token()->sourceCodeRef.position();
        consume();
        if(is(Token::LeftParenthesis)) {
            openToken();
            consume();
            consumeNewline();
            if(!is(Token::RightParenthesis))
                message->inputs()->append(scanExpression());
            else
                message->inputs(); // create an empty inputs argument bunch
            closeToken();
            match(Token::RightParenthesis);
        }
//            if(isNestedBlock()) message->setBlock(Block::cast(scanNestedBlock()->value()));
        int end = token()->sourceCodeRef.position();
        primitive->setSourceCodeRef(lexer()->source().midRef(begin, end - begin));
        consumeUselessNewline();
        return primitive;
    }

    bool Parser::isLiteral() const {
        Token::Type type = tokenType();
        return type == Token::Boolean || type == Token::Number ||
               type == Token::Character || type == Token::Text;
    }

    Primitive *Parser::scanLiteral() {
        Node *value;
        QChar c;
        QString s;
        switch(tokenType()) {
        case Token::Boolean:
            value = LIU_BOOLEAN(tokenText() == "yes" || tokenText() == "true");
            break;
        case Token::Number:
            value = LIU_NUMBER(tokenText().toDouble());
            break;
        case Token::Character:
            c = Text::unescapeSequence(tokenText().mid(1, tokenText().size() - 2)).at(0);
            value = LIU_CHARACTER(c);
            break;
        case Token::Text: {
            QList<IntPair> interpolableSlices;
            try {
                s = Text::unescapeSequence(tokenText().mid(1, tokenText().size() - 2), &interpolableSlices);
            } catch(const Exception &e) {
                throw parserException(e.message);
            }
            Text *text = LIU_TEXT(s);
            // text->setIsTranslatable(true);
            if(!interpolableSlices.isEmpty()) text->setInterpolableSlices(interpolableSlices);
            value = text;
            break;
        }
        default:
            throw parserException("unimplemented token");
        }
        Primitive *primitive = LIU_PRIMITIVE(value, token()->sourceCodeRef);
        consume();
        consumeUselessNewline();
        return primitive;
    }

    Primitive *Parser::scanCollection() {
        Message *message = LIU_MESSAGE("make");
        int begin = token()->sourceCodeRef.position();
        openToken();
        consume(); // Left bracket
        consumeNewline();
        if(!is(Token::RightBracket))
            message->inputs()->append(scanExpression());
        else
            message->inputs(); // create an empty inputs argument bunch
        bool hasKey = false;
        for(int i = 0; i < message->numInputs(); ++i)
            if(message->input(i)->label()) {
                hasKey = true;
                break;
            }
        int end = token()->sourceCodeRef.position() + 1;
        closeToken();
        QString rightBracket = match(Token::RightBracket);
        if(rightBracket.contains("?")) message->setIsQuestioned(true);
        if(rightBracket.contains("!")) message->setIsExclaimed(true);
        QStringRef sourceCodeRef = lexer()->source().midRef(begin, end - begin);
        Primitive *primitive = LIU_PRIMITIVE(LIU_MESSAGE(!hasKey ? "List" : "Dictionary"), sourceCodeRef);
        primitive->setNext(LIU_PRIMITIVE(message, sourceCodeRef));
        return primitive;
    }

    Primitive *Parser::scanSubexpression() {
        Primitive *primitive = LIU_PRIMITIVE();
        int begin = token()->sourceCodeRef.position();
        openToken();
        consume(); // Left parenthesis
        consumeNewline();
        primitive->setValue(scanExpression());
        int end = token()->sourceCodeRef.position() + 1;
        closeToken();
        match(Token::RightParenthesis);
        primitive->setSourceCodeRef(lexer()->source().midRef(begin, end - begin));
        return primitive;
    }

    Primitive *Parser::scanNestedBlock() {
        Primitive *primitive = LIU_PRIMITIVE();
        int begin = token()->sourceCodeRef.position();
        openToken();
        consume(); // Left brace
        consumeNewline();
        primitive->setValue(scanBlock(Token::RightBrace));
        int end = token()->sourceCodeRef.position() + 1;
        closeToken();
        match(Token::RightBrace);
        primitive->setSourceCodeRef(lexer()->source().midRef(begin, end - begin));
        return primitive;
    }

    Primitive *Parser::scanCollectionAccessor(Primitive *currentPrimitive) {
        Message *message = LIU_MESSAGE("[]");
        int begin = token()->sourceCodeRef.position();
        openToken();
        consume(); // Left bracket
        consumeNewline();
        if(!is(Token::RightBracket))
            message->inputs()->append(scanExpression());
        int end = token()->sourceCodeRef.position() + 1;
        closeToken();
        QString rightBracket = match(Token::RightBracket);
        if(rightBracket.contains("?")) message->setIsQuestioned(true);
        if(rightBracket.contains("!")) message->setIsExclaimed(true);
        QStringRef sourceCodeRef = lexer()->source().midRef(begin, end - begin);
        LIU_PRIMITIVE_ADD(currentPrimitive, LIU_PRIMITIVE(message, sourceCodeRef));
        return currentPrimitive;
    }

    Operator *Parser::isOperator(Operator::Type type) const {
        if(!is(Token::Operator)) return NULL;
        return lexer()->operatorTable()->has(tokenText(), type);
    }

    Primitive *Parser::scanPrefixOperator(Primitive *currentPrimitive, Operator *currentOp) {
        QStringRef sourceCodeRef = token()->sourceCodeRef;
        consume();
        consumeNewline();
        Primitive *chain = NULL;
        if(currentOp->isSpecial) {
            if(currentOp->name == "@") {
                chain = scanUnaryExpression(NULL);
                Message *msg = Message::dynamicCast(chain->value());
                if(!msg) throw parserException("missing message after '@' operator");
                msg->setIsParented(true);
            } else if(currentOp->name == "\\") {
                chain = scanUnaryExpression(NULL);
                Message *msg = Message::dynamicCast(chain->value());
                if(!msg) throw parserException("missing message after '\\' operator");
                msg->setIsEscaped(true);
            } else if(currentOp->name == "value:") {
                Primitive *key = LIU_PRIMITIVE(LIU_MESSAGE("value"), sourceCodeRef);
                chain = LIU_PRIMITIVE(LIU_PAIR(key, scanExpression()), sourceCodeRef);
            } else if(currentOp->name == "?:" || currentOp->name == "!:") {
                chain = LIU_PRIMITIVE(LIU_MESSAGE(currentOp->name), sourceCodeRef);
                LIU_PRIMITIVE_ADD(chain, scanExpression());
            } else
                throw parserException("unimplemented special operator");
        } else {
            chain = scanUnaryExpression(NULL);
            LIU_PRIMITIVE_ADD(chain, LIU_PRIMITIVE(LIU_MESSAGE(currentOp->name), sourceCodeRef));
        }
        LIU_PRIMITIVE_ADD(currentPrimitive, chain);
        return currentPrimitive;
    }

    Primitive *Parser::scanPostfixOperator(Primitive *currentPrimitive, Operator *currentOp) {
        if(currentOp->isSpecial) {
            if(currentOp->name == "...") {
                if(!currentPrimitive) throw parserException("missing primitive before '...'");
                Message *message = Message::dynamicCast(currentPrimitive->last()->value());
                if(!message) throw parserException("missing message before '...'");
                if(currentPrimitive->hasNext()) {
                    QString name = message->name();
                    Message *methodSignature = Message::dynamicCast(currentPrimitive->last()->previous()->value());
                    if(!methodSignature) throw parserException("missing method signature before '...'");
                    methodSignature->setCodeInputName(name);
                    currentPrimitive->last()->previous()->setNext(NULL);
                } else
                    message->setIsEllipsed(true);
            } else
                throw parserException("unimplemented special operator");
        } else {
            if(currentOp->useLHSAsReceiver) {
                LIU_PRIMITIVE_ADD(currentPrimitive,
                                    LIU_PRIMITIVE(LIU_MESSAGE(currentOp->name), token()->sourceCodeRef));
            } else {
                if(!currentPrimitive) throw parserException(QString("missing primitive before '%1'").arg(currentOp->name));
                Message *message = LIU_MESSAGE(currentOp->name);
                message->inputs()->append(currentPrimitive->last());
                Primitive *primitive = LIU_PRIMITIVE(message, token()->sourceCodeRef);
                if(currentPrimitive->hasNext())
                    currentPrimitive->last()->previous()->setNext(primitive);
                else
                    currentPrimitive = primitive;
            }
        }
        consume();
        consumeUselessNewline();
        return currentPrimitive;
    }

    Primitive *Parser::scanBinaryOperator(Primitive *lhs, Operator *currentOp, const short minPrecedence) {
        do {
            QStringRef sourceCodeRef = token()->sourceCodeRef;
            consume();
            if(currentOp->name != ":") consumeNewline();
            Primitive *rhs = scanUnaryExpressionChain();
            Operator *nextOp;
            while((nextOp = isBinaryOperator()) && (
                      operatorPrecedence(nextOp) > operatorPrecedence(currentOp) ||
                      (nextOp->associativity == Operator::RightAssociative &&
                       operatorPrecedence(nextOp) == operatorPrecedence(currentOp))
                      )) {
                rhs = scanBinaryOperator(rhs, nextOp, operatorPrecedence(nextOp));
            }
            lhs = resolveBinaryOperator(lhs, currentOp, rhs, sourceCodeRef);
        } while((currentOp = isBinaryOperator()) && operatorPrecedence(currentOp) >= minPrecedence);
        return lhs;
    }

    Primitive *Parser::resolveBinaryOperator(Primitive *lhs,
                                     const Operator *op,
                                     Primitive *rhs,
                                     const QStringRef &sourceCodeRef) {
        if(op->isSpecial) {
            if(op->name == ":")
                lhs = LIU_PRIMITIVE(LIU_PAIR(lhs, rhs), sourceCodeRef);
            else if(op->name == ",") {
                checkRightHandSide(rhs);
                Bunch *bunch = Bunch::dynamicCast(lhs->value());
                if(!bunch)
                    lhs = LIU_PRIMITIVE(LIU_BUNCH(lhs, rhs), sourceCodeRef);
                else
                    bunch->append(rhs);
            } else if(op->name == "->") {
                checkRightHandSide(rhs);
                Message *message = Message::dynamicCast(lhs->last()->value());
                if(!message) throw parserException("message expected before '->'");
                message->outputs()->append(rhs);
            } else
                throw parserException("unimplemented special operator");
        } else {
            checkRightHandSide(rhs);
            if(op->useLHSAsReceiver) {
                Message *message = LIU_MESSAGE(op->name);
                message->inputs()->append(rhs);
                LIU_PRIMITIVE_ADD(lhs, LIU_PRIMITIVE(message, sourceCodeRef));
            } else {
                Message *message = LIU_MESSAGE(op->name);
                message->inputs()->append(lhs->last());
                message->inputs()->append(rhs);
                Primitive *primitive = LIU_PRIMITIVE(message, sourceCodeRef);
                if(lhs->hasNext())
                    lhs->last()->previous()->setNext(primitive);
                else
                    lhs = primitive;
            }
        }
        return lhs;
    }

    short Parser::operatorPrecedence(const Operator *op) const {
        short precedence = op->precedence;
        if(op->name == ",") {
            Token::Type type = topToken();
            if(type == Token::LeftParenthesis || type == Token::LeftBracket)
                precedence = 1;
        }
        return precedence;
    }

    ParserException Parser::parserException(QString message) const {
        int column, line;
        computeColumnAndLineForPosition(lexer()->source(), token()->sourceCodeRef.position(), column, line);
        QString text = extractLine(lexer()->source(), line);
        if(!text.isEmpty()) {
            QString cursor = QString(" ").repeated(column - 1).append("^");
            message += "\n" + text + "\n" + cursor;
        }
        return ParserException(context()->child("ParserException"), message, lexer()->resourceName(), line);
    }
}

LIU_END
