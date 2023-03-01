#include "Log.h"
#include "DxLib.h"

void Log(wstring msg)
{
#ifdef _DEBUG
	SYSTEMTIME st;
	GetLocalTime(&st);

	const auto time =
		to_wstring((int)st.wYear) + L"-" +
		to_wstring((int)st.wMonth) + L"-" +
		to_wstring((int)st.wDay) + L"T" +
		to_wstring((int)st.wHour) + L":" +
		to_wstring((int)st.wMinute) + L":" +
		to_wstring((int)st.wSecond) + L"." +
		to_wstring((int)st.wMilliseconds) + L" ";

	OutputDebugString((time + msg + L"\n").c_str());
#endif // DEBUG
}
