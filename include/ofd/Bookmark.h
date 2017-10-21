#ifndef __OFD_BOOKMARK_H__
#define __OFD_BOOKMARK_H__

#include "ofd/Common.h"
#include "utils/xml.h"

namespace ofd{

    class Bookmark : public utils::XMLable {
    public:
        Bookmark();
        virtual ~Bookmark();

        virtual void GenerateXML(utils::XMLWriter &writer) const override;
        virtual bool FromXML(utils::XMLElementPtr element) override;

    public:
        std::string Name;
        ST_Dest Dest;

    }; // class Bookmark

    class Bookmarks : public utils::XMLable {
    public:
        Bookmarks();
        virtual ~Bookmarks();

        virtual void GenerateXML(utils::XMLWriter &writer) const override;
        virtual bool FromXML(utils::XMLElementPtr element) override;

    public:

    }; // class Bookmarks

}; // namespace ofd

#endif // __OFD_BOOKMARK_H__
