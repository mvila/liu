#ifndef LIU_LANGUAGE_BLOCK_H
#define LIU_LANGUAGE_BLOCK_H

#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    #define LIU_BLOCK(ARGS...) \
    new Language::Block(context()->child("Object", "Language", "Block"), ##ARGS)

    class Block : public GenericList<Section *> {
        LIU_DECLARE(Block, OldList, Language);
    public:
        explicit Block(Node *origin) : GenericList<Section *>(origin),
            _doc(NULL), _docIsCached(false), _body(NULL), _bodyIsCached(false), _test(NULL), _testIsCached(false),
            _else(NULL), _elseIsCached(false), _between(NULL), _betweenIsCached(false), _metaSectionsHaveBeenRun(false)  {}

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(Block);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(Block);

        virtual bool isDefined(QSet<const Node *> *alreadySeen) const { Q_UNUSED(alreadySeen); return true; } // TODO

        virtual Node *run(Node *receiver = context());

        void runMetaSections(Node *receiver);

        Section *section(const QString &label);
        Section *docSection();
        Section *bodySection();
        Section *testSection();
        Section *elseSection();
        Section *betweenSection();
    private:
        Section *findSection(const QString &label);
    public:
        Section *hasUnlabeledSection();

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        Section *_doc;
        bool _docIsCached;
        Section *_body;
        bool _bodyIsCached;
        Section *_test;
        bool _testIsCached;
        Section *_else;
        bool _elseIsCached;
        Section *_between;
        bool _betweenIsCached;
        bool _metaSectionsHaveBeenRun;
    };
}

LIU_END

#endif // LIU_LANGUAGE_BLOCK_H
