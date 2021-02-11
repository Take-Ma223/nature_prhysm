#include"ShowErrorName.h"
#include"DxLib.h"

void ShowError(wchar_t* str) {
	printfDx(L"%s\n", str);
	ScreenFlip();
	Sleep(10000);
}