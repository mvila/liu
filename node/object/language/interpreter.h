#ifndef LIU_LANGUAGE_INTERPRETER_H
#define LIU_LANGUAGE_INTERPRETER_H

#include "node/object/language/operatortable.h"
#include "node/object/language/lexer.h"
#include "node/object/language/parser.h"
#include "node/object/language/sourcecodedictionary.h"
#include "node/object/language/testsuite.h"

LIU_BEGIN

namespace Language {
    #define LIU_INTERPRETER(ARGS...) \
    new Language::Interpreter(context()->child("Object", "Language", "Interpreter"), ##ARGS)

    class Interpreter : public Object {
        LIU_DECLARE(Interpreter, Object, Language);
    public:
        explicit Interpreter(Node *origin) :
            Object(origin), _operatorTable(NULL), _lexer(NULL), _parser(NULL), _sourceCodes(NULL) {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Interpreter);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Interpreter); // TODO

        void initOperatorTable();

        OperatorTable *operatorTable() const { return _operatorTable; }
        Lexer *lexer() const { return _lexer; }
        Parser *parser() const { return _parser; }

        SourceCodeDictionary *sourceCodes() const { return _sourceCodes; }
        SourceCode *loadSourceCode(QString url);
        Node *runSourceCode(QString url, Node *receiver = context()) { return loadSourceCode(url)->run(receiver); }
        SourceCode *sourceCodeIsAlreadyLoaded(QString url);

        TestSuite *testSuite() const { return TestSuite::cast(constCast(this)->child("test_suite")); }
        void setTestSuite(TestSuite *testSuite) { addOrSetChild("test_suite", testSuite); }
    private:
        OperatorTable *_operatorTable;
        Lexer *_lexer;
        Parser *_parser;
        SourceCodeDictionary *_sourceCodes;
    };
}

LIU_END

#endif // LIU_LANGUAGE_INTERPRETER_H
