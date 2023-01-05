#pragma once
#include <unordered_map>
#include <string>
#include "FontHandle.h"

class FontSet
{
	std::unordered_map<FontInfo, FontHandle, FontInfoHash> font;//(�t�H���g���,�n���h�����)�@�̎���
	void registFont(FontInfo);
public:
	~FontSet();
	FontHandle getFontHandle(FontInfo);

	void deleteFont();
};

