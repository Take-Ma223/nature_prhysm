#include <string>
#include <DxLib.h>
#include "STRUCT_NOTE.h"
#include "STRUCT_CONFIG.h"

using namespace std;

#pragma once
class NoteSymbolImage
{
	wstring fileName[10] = {
		wstring(L""),
		wstring(L"r.png"),
		wstring(L"g.png"),
		wstring(L"b.png"),
		wstring(L"c.png"),
		wstring(L"m.png"),
		wstring(L"y.png"),
		wstring(L"w.png"),
		wstring(L"k.png"),
		wstring(L"f.png"),
	};

	wstring rootPath = wstring(L"img/note_symbol");

public:
	int H_NOTE_TEXT[10];

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="config">config</param>
	/// <param name="folder">どのnote symbolを使うかを決めるフォルダパス</param>

	NoteSymbolImage(Config& config, wstring folder) {
		if (config.NoteText[0] == 1) loadImage(NoteColor::R, folder);
		if (config.NoteText[1] == 1) loadImage(NoteColor::G, folder);
		if (config.NoteText[2] == 1) loadImage(NoteColor::B, folder);
		if (config.NoteText[3] == 1) loadImage(NoteColor::C, folder);
		if (config.NoteText[4] == 1) loadImage(NoteColor::M, folder);
		if (config.NoteText[5] == 1) loadImage(NoteColor::Y, folder);
		if (config.NoteText[6] == 1) loadImage(NoteColor::W, folder);
		if (config.NoteText[7] == 1) loadImage(NoteColor::K, folder);
		if (config.NoteText[8] == 1) loadImage(NoteColor::F, folder);
	}

	void loadImage(NoteColor noteColor, wstring folder) {
		H_NOTE_TEXT[(int)noteColor] = LoadGraph((rootPath + L"/" + folder + L"/" + fileName[(int)noteColor]).c_str());
	}
};

