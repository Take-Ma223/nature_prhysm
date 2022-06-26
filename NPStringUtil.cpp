#include "NPStringUtil.h"

//最初の:で文字列を分割
void NPStringUtil::devideByFirstColon(wstring input, wstring* output1, wstring* output2)
{
	//コロンの位置を探す
	int colonIndex = input.find_first_of(':');

	if (colonIndex == wstring::npos) {
		*output1 = input;
		*output2 = wstring(L"");
	}
	else {
		*output1 = input.substr(0, colonIndex);
		*output2 = input.substr(colonIndex + 1);
	}



}
