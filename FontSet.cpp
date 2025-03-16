#include "FontSet.h"
#include <stdexcept>

void FontSet::registFont(FontInfo fontInfo)
{
	int handle = CreateFontToHandle(
		fontInfo.fontName.c_str(),
		fontInfo.size,
		fontInfo.thick,
		(int)(fontInfo.fontType),
		-1,
		fontInfo.edge_size
	);
	if (handle == -1) {
		throw std::runtime_error("�t�H���g�̓ǂݍ��݂Ɏ��s���܂���");
	}
	font[fontInfo] = FontHandle(handle, fontInfo); //�t�H���g���ƃn���h���̓o�^
}

FontSet::~FontSet()
{
	deleteFont();
}

FontHandle FontSet::getFontHandle(FontInfo fontInfo)
{
	if (font.find(fontInfo) != font.end()) {
		//���ɓǂ݂��񂾃t�H���g
	}
	else {
		//���ǂݍ��݃t�H���g
		registFont(fontInfo);
	}
	return font[fontInfo];
}

void FontSet::deleteFont()
{
	for (auto itr = font.begin(); itr != font.end(); ++itr) {
		DeleteFontToHandle(itr->second.getHandle());
	}
}
