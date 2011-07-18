#include "node/object/language/token.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Token, Object, Language);

    const char *Token::typeNames[] = {
        "<NULL>",
        "<EOF>",
        "Name",
        "Operator",
        "Boolean",
        "Number",
        "Character",
        "Text",
        "LeftParenthesis",
        "RightParenthesis",
        "LeftBracket",
        "RightBracket",
        "LeftBrace",
        "RightBrace",
        "Semicolon",
        "Newline"
    };

    void Token::initRoot() {
    }
}

LIU_END
