#include "FontSet.h"

void FontSet::registFont(FontInfo fontInfo)
{
	int handle = CreateFontToHandle(
		fontInfo.fontName.c_str(),
		fontInfo.size,
		fontInfo.thick,
		(int)(fontInfo.fontType)
	);
	font[fontInfo] = FontHandle(handle, fontInfo); //フォント情報とハンドルの登録
}

FontSet::~FontSet()
{
	deleteFont();
}

FontHandle FontSet::getFontHandle(FontInfo fontInfo)
{
	if (font.find(fontInfo) != font.end()) {
		//既に読みこんだフォント
	}
	else {
		//初読み込みフォント
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
