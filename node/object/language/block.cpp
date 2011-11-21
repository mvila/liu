#include "node/object/language/block.h"
#include "node/object/language/message.h"
#include "node/object/language/testsuite.h"
#include "node/object/language/abstractmethod.h"

LIU_BEGIN

namespace Language {
    LIU_DEFINE_2(Block, Object, Language);

    Block *Block::init(List *sections) {
        Object::init();
        setSections(sections);
        return this;
    }

    Block *Block::initCopy(const Block *other) {
        Object::initCopy(other);
        setSections(other->_sections);
        return this;
    }

    Block::~Block() {
        setSections();
        deleteCachedLabels();
    }

    void Block::initRoot() {
        setSections(List::root());
    }

    LIU_DEFINE_NODE_ACCESSOR(Block, List, sections, Sections);
    LIU_DEFINE_EMPTY_ACCESSOR_CALLBACKS(Block, sections);

    Node *Block::unnamedChild(int index) const {
        if(_sections && _sections->isReal()) {
            if(index == 0) return _sections;
            index--;
        }
        return NULL;
    }

    void Block::hasChanged() {
        deleteCachedLabels();
        _metaSectionsHaveBeenRun = false;
        Object::hasChanged();
    }

    Node *Block::run(Node *receiver) {
        runMetaSections(receiver);
        if(Section *body = section("body")) {
            LIU_PUSH_RUN(this);
            return body->run(receiver);
        } else
            return receiver;
    }

    void Block::runMetaSections(Node *receiver) {
        if(!_metaSectionsHaveBeenRun) {
            if(Section *doc = section("doc")) receiver->addOrSetChild(".doc", doc->run(receiver));
            if(Section *inputsSection = section("inputs")) {
                AbstractMethod *method = AbstractMethod::dynamicCast(receiver);
                if(!method) LIU_THROW_RUNTIME_EXCEPTION("'inputs' section is reserved to Method objects");
                ArgumentBunch *inputs = LIU_ARGUMENT_BUNCH();
                if(!inputsSection->lines()->isEmpty()) {
                    if(inputsSection->lines()->size() > 1) LIU_THROW_RUNTIME_EXCEPTION("'inputs' section has more than one line");
                    Primitive *prim = Primitive::cast(inputsSection->lines()->first().second);
                    Primitive *prim2 = Primitive::dynamicCast(prim->value());
                    if(prim2) prim = prim2;
                    inputs->append(prim);
                }
                method->setInputs(inputs);
            }
            if(Section *test = section("test")) {
                TestSuite *testSuite = TestSuite::cast(child("test_suite"));
                testSuite->append(Test::make(test, receiver));
            }
            _metaSectionsHaveBeenRun = true;
        }
    }

    Section *Block::section(const QString &label) const {
        Section *result = findSection(label);
        if(!result && ((label == "doc" && findSection("body")) || label == "body"))
            result = findSection("");
        return result;
    }

    Section *Block::findSection(const QString &label) const {
        return cachedLabels()->value(label);
    }

    QHash<QString, Section *> *Block::cachedLabels() const {
        if(!_cachedLabels) {
            _cachedLabels = new QHash<QString, Section *>;
            bool first = true;
            QScopedPointer<List::Iterator> i(sections()->iterator());
            while(i->hasNext()) {
                Section *section = Section::cast(i->next().second);
                if(section->hasLabel()) {
                    if(!section->label()->hasNext()) {
                        if(Message *message = Message::dynamicCast(section->label()->value())) {
                            if(_cachedLabels->contains(message->name()))
                                LIU_THROW(DuplicateException, "a section label is duplicate");
                            _cachedLabels->insert(message->name(), section);
                        }
                    }
                } else if(first) {
                    _cachedLabels->insert("", section);
                } else
                    LIU_THROW(ParserException, "unlabeled section found not in first position");
                first = false;
            }
        }
        return _cachedLabels;
    }

    QString Block::toString(bool debug, short level) const {
        QString str;
        if(level > 0) str += "{\n";
        str += sections()->join("\n", "", "", debug, level + 1);
        if(level > 0) str += "\n" + QString("    ").repeated(level) + "}";
        return str;
    }
}

LIU_END
