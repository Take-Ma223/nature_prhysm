#include"FontInfo.h"

FontInfo::FontInfo(wstring fontName, int size, int thick, FontType fontType, int edge_size)
{
	FontInfo::fontName = fontName;
	FontInfo::size = size;
	FontInfo::thick = thick;
	FontInfo::fontType = fontType;
	FontInfo::edge_size = edge_size;

}
