#include"FontInfo.h"

FontInfo::FontInfo(wstring fontName, int size, int thick, FontType fontType)
{
	FontInfo::fontName = fontName;
	FontInfo::size = size;
	FontInfo::thick = thick;
	FontInfo::fontType = fontType;
}
