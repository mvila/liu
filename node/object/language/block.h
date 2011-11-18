#ifndef LIU_LANGUAGE_BLOCK_H
#define LIU_LANGUAGE_BLOCK_H

#include "node/object/language/section.h"

LIU_BEGIN

namespace Language {
    class Block : public Object {
        LIU_DECLARE_2(Block, Object, Language);
    public:
        explicit Block(Node *origin = context()->child("Object", "Language", "Block")) :
            Object(origin), _sections(NULL), _cachedLabels(NULL), _metaSectionsHaveBeenRun(false)  {}

        Block *init(List *sections = NULL);

        LIU_DECLARE_NODE_ACCESSOR(List, sections, Sections);

        virtual bool isDefined(QSet<const Node *> *alreadySeen) const { Q_UNUSED(alreadySeen); return true; } // TODO

        virtual Node *unnamedChild(int index) const;

        virtual void hasChanged();

        virtual Node *run(Node *receiver = context());

        void runMetaSections(Node *receiver);

        Section *section(const QString &label) const;
    private:
        Section *findSection(const QString &label) const;
        QHash<QString, Section *> *cachedLabels() const;
        void deleteCachedLabels() const { delete _cachedLabels; _cachedLabels = NULL; }
    public:
        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        List *_sections;
        mutable QHash<QString, Section *> *_cachedLabels;
        bool _metaSectionsHaveBeenRun;
    };
}

LIU_END

#endif // LIU_LANGUAGE_BLOCK_H
