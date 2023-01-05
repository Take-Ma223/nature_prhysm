#pragma once
#include<string>
#include <DxLib.h>
#include "FontInfo.h"
using namespace std;

/**
* �t�H���g�n���h���N���X
*/
class FontHandle
{
	int handle;
	FontInfo fontInfo;
public:
	FontHandle(int handle = 0, FontInfo fontInfo = FontInfo());

	int getHandle() { return handle; };
	FontInfo getFontInfo() { return fontInfo; };

	wstring getFontName() { return fontInfo.fontName; };
	int getSize() { return fontInfo.size; };
	int getThick() { return fontInfo.thick; };
	FontType getFontType() { return fontInfo.fontType; };
};

