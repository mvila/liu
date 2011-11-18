#ifndef LIU_LANGUAGE_SOURCE_CODE_H
#define LIU_LANGUAGE_SOURCE_CODE_H

#include "node/object/text.h"
#include "node/object/language/block.h"
#include "node/object/language/parser.h"

LIU_BEGIN

namespace Language {
    #define LIU_SOURCE_CODE(ARGS...) \
    new Language::SourceCode(context()->child("Object", "Language", "SourceCode"), ##ARGS)

    class SourceCode : public Object {
        LIU_DECLARE(SourceCode, Object, Language);
    public:
        explicit SourceCode(Node *origin, const QString &url = "",
                   const QString &txt = "", Block *block = NULL);

        virtual ~SourceCode() {
            LIU_UNSET_FIELD(_block);
        }

        LIU_DECLARE_AND_DEFINE_COPY_METHOD(SourceCode);
        LIU_DECLARE_AND_DEFINE_FORK_METHOD(SourceCode, url(), text(), LIU_FORK_IF_NOT_NULL(block()));

        const QString &url() const { return _url; }
        void setUrl(const QString &url) { _url = normalizeUrl(url); }

        const QString &text() const { return _text; }
        void setText(const QString &text) { _text = text; }

        Block *block() const { return _block; }

        void setBlock(Block *block) {
            LIU_SET_FIELD(_block, block);
        }

        void load(const QString &newUrl = "");
        void parse(const QString &newText = "");

        virtual Node *run(Node *receiver = context()) {
            LIU_PUSH_RUN(this);
            if(!block()) return NULL;
            Section *body = block()->section("body");
            if(!body) return NULL;
            return body->run(receiver);
        }

        virtual QString toString(bool debug = false, short level = 0) const;
    private:
        QString _url;
        QString _text;
        Block *_block;
    };
}

LIU_END

#endif // LIU_LANGUAGE_SOURCE_CODE_H
