#include "NPStringUtil.h"

//�ŏ���:�ŕ�����𕪊�
void NPStringUtil::devideByFirstColon(wstring input, wstring* output1, wstring* output2)
{
	//�R�����̈ʒu��T��
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
