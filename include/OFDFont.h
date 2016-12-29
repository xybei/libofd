#ifndef __OFDFONT_H__
#define __OFDFONT_H__

#include <string>
#include <vector>
#include <map>
#include <memory>

struct _cairo_font_face;

namespace utils{
    class XMLWriter;
    class XMLElement;
    typedef std::shared_ptr<XMLElement> XMLElementPtr;
}; // namespace utils

namespace ofd {

    namespace Font{

        enum class Type {
            Unknown = -1,
            TrueType,
            CIDType2,
            Type1,
            Type3,
        };

        enum class Location{
            Unknown = -1,
            Embedded,
            External,
            Resident,
        };
    };

    struct OFDFont;
    typedef std::shared_ptr<OFDFont> OFDFontPtr;

    // ======== class OFDFont ========
    // OFD (section 11.1) P61. Res.xsd.
    typedef struct OFDFont {
    public:
        OFDFont();
        ~OFDFont();

    public:
        uint64_t    ID;
        std::string FontName;
        std::string FamilyName;
        std::string Charset;
        bool        Serif;
        bool        Bold;
        bool        Italic;
        bool        FixedWidth;
        std::string FontFile;

        Font::Type FontType;
        Font::Location FontLoc;

        char *m_fontData;
        size_t m_fontDataSize;

        _cairo_font_face *font_face;

        std::string ToString() const;
        void GenerateXML(utils::XMLWriter &writer) const;
        bool FromXML(utils::XMLElementPtr fontElement);
        std::string GetFileName() const;

    } OFDFont_t; // class OFDFont

    typedef std::vector<OFDFontPtr> FontArray;
    typedef std::map<uint64_t, OFDFontPtr> FontMap;

}; // namespace ofd

#endif // __OFDFONT_H__
