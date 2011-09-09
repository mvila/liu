#include "node/object/language/block.h"
#include "node/object/language/message.h"
#include "node/object/language/testsuite.h"
#include "node/object/language/abstractmethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE(Block, List, Language);

    void Block::initRoot() {
    }

    Node *Block::run(Node *receiver) {
        runMetaSections(receiver);
        if(bodySection()) {
            LIU_PUSH_RUN(this);
            return bodySection()->run(receiver);
        } else
            return receiver;
    }

    void Block::runMetaSections(Node *receiver) {
        if(!_metaSectionsHaveBeenRun) {
            if(docSection()) receiver->addOrSetChild("doc", docSection()->run(receiver));
            if(Section *inputsSection = section("inputs")) {
                AbstractMethod *method = AbstractMethod::dynamicCast(receiver);
                if(!method) LIU_THROW_RUNTIME_EXCEPTION("'inputs' section is reserved to Method objects");
                ArgumentBunch *inputs = LIU_ARGUMENT_BUNCH();
                if(!inputsSection->isEmpty()) {
                    if(inputsSection->size() > 1) LIU_THROW_RUNTIME_EXCEPTION("'inputs' section has more than one line");
                    Primitive *prim = inputsSection->get(0);
                    Primitive *prim2 = Primitive::dynamicCast(prim->value());
                    if(prim2) prim = prim2;
                    inputs->append(prim);
                }
                method->setInputs(inputs);
            }
            if(testSection()) {
                TestSuite *testSuite = TestSuite::cast(child("test_suite"));
                testSuite->append(LIU_TEST(testSection(), receiver));
            }
            _metaSectionsHaveBeenRun = true;
        }
    }

    Section *Block::section(const QString &label) {
        if(label.isEmpty()) return hasUnlabeledSection();
        else if(label == "doc") return docSection();
        else if(label == "body") return bodySection();
        else if(label == "test") return testSection();
        else if(label == "else") return elseSection();
        else if(label == "between") return betweenSection();
        else return findSection(label);
    }

    Section *Block::docSection() {
        if(!_docIsCached) {
            _doc = findSection("doc");
            if(!_doc && findSection("body")) _doc = hasUnlabeledSection();
            _docIsCached = true;
        }
        return _doc;
    }

    Section *Block::bodySection() {
        if(!_bodyIsCached) {
            _body = findSection("body");
            if(!_body) _body = hasUnlabeledSection();
            _bodyIsCached = true;
        }
        return _body;
    }

    Section *Block::testSection() {
        if(!_testIsCached) {
            _test = findSection("test");
            _testIsCached = true;
        }
        return _test;
    }

    Section *Block::elseSection() {
        if(!_elseIsCached) {
            _else = findSection("else");
            _elseIsCached = true;
        }
        return _else;
    }

    Section *Block::betweenSection() {
        if(!_betweenIsCached) {
            _between = findSection("between");
            _betweenIsCached = true;
        }
        return _between;
    }

    Section *Block::findSection(const QString &label) {
        Iterator i(this);
        while(Section *section = i.next()) {
            if(section->label() && !section->label()->hasNext()) {
                Message *message = Message::dynamicCast(section->label()->value());
                if(message && message->name() == label) return section;
            }
        }
        return NULL;
    }

    Section *Block::hasUnlabeledSection() {
        if(isNotEmpty() && !first()->label())
            return first();
        else
            return NULL;
    }

    QString Block::toString(bool debug, short level) const {
        QString str;
        if(level > 0) str += "{\n";
        str += join("\n", "", "", debug, level + 1);
        if(level > 0) str += "\n" + QString("    ").repeated(level) + "}";
        return str;
    }
}

LIU_END
