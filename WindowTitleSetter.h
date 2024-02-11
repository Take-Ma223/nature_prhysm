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
	/// �E�B���h�E�^�C�g����ύX
	/// </summary>
	static void setText(ShowFlag flag) {
		std::wstring version = flag.version ? std::wstring(L"nature prhysm ver ") + APPLICATION_VERSION : L"";
		std::wstring autoPlay = flag.autoPlay ? std::wstring(L"F1:�I�[�g�v���C") : L"";
		std::wstring scoreRanking = flag.scoreRanking ? std::wstring(L"F2:�X�R�A�����L���O�\��") : L"";
		std::wstring keyConfig = flag.keyConfig ? std::wstring(L"F3:�L�[�R���t�B�O") : L"";
		std::wstring twitter = flag.twitter ? std::wstring(L"F11:Twitter�ɓ��e") : L"";
		std::wstring screenShot = flag.screenShot ? std::wstring(L"PrtSc:�X�N���[���V���b�g") : L"";

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

