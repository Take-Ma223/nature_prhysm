#pragma once
#include <unordered_map>
#include <string>
#include "FontHandle.h"

class FontSet
{
	std::unordered_map<FontInfo, FontHandle, FontInfoHash> font;//(フォント情報,ハンドル情報)　の辞書
	void registFont(FontInfo);
public:
	~FontSet();
	FontHandle getFontHandle(FontInfo);

	void deleteFont();
};

