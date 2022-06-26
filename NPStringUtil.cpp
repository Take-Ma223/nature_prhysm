#include "NPStringUtil.h"

//Å‰‚Ì:‚Å•¶š—ñ‚ğ•ªŠ„
void NPStringUtil::devideByFirstColon(wstring input, wstring* output1, wstring* output2)
{
	//ƒRƒƒ“‚ÌˆÊ’u‚ğ’T‚·
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
