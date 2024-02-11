#pragma once
#include "CONSTANT_VALUE.h"
#include "DxLib.h"
#include <string>
#include <vector>

struct ShowFlag{
	bool version = false;
	bool autoPlay = false;
	bool scoreRanking = false;
	bool keyConfig = false;
	bool twitter = false;
	bool screenShot = false;
};

class WindowTitleSetter
{
public:
	/// <summary>
	/// ウィンドウタイトルを変更
	/// </summary>
	static void setText(ShowFlag flag) {
		std::wstring version = flag.version ? std::wstring(L"nature prhysm ver ") + APPLICATION_VERSION : L"";
		std::wstring autoPlay = flag.autoPlay ? std::wstring(L"F1:オートプレイ") : L"";
		std::wstring scoreRanking = flag.scoreRanking ? std::wstring(L"F2:スコアランキング表示") : L"";
		std::wstring keyConfig = flag.keyConfig ? std::wstring(L"F3:キーコンフィグ") : L"";
		std::wstring twitter = flag.twitter ? std::wstring(L"F11:Twitterに投稿") : L"";
		std::wstring screenShot = flag.screenShot ? std::wstring(L"PrtSc:スクリーンショット") : L"";

		std::vector<std::wstring> textArray = { version,autoPlay,scoreRanking,keyConfig,twitter,screenShot };

		std::wstring text;
		std::wstring textSpace = L"   ";

		for (const auto& t : textArray) {
			if (t == std::wstring(L""))continue;
			text += t + textSpace;
		}


		SetMainWindowText(text.c_str());
	}





};

