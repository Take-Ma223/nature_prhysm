#include <string>
#include <DxLib.h>
#include "STRUCT_NOTE.h"
#include "STRUCT_CONFIG.h"

using namespace std;

#pragma once
class NoteTextImage
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

	wstring rootPath = wstring(L"img/note_text");

public:
	int H_NOTE_TEXT[10];

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="path">ノーツテキスト画像へのパス img/notes/XXX</param>
	NoteTextImage(Config& config) {
		if (config.NoteText[0] == 1) loadImage(NoteColor::R);
		if (config.NoteText[1] == 1) loadImage(NoteColor::G);
		if (config.NoteText[2] == 1) loadImage(NoteColor::B);
		if (config.NoteText[3] == 1) loadImage(NoteColor::C);
		if (config.NoteText[4] == 1) loadImage(NoteColor::M);
		if (config.NoteText[5] == 1) loadImage(NoteColor::Y);
		if (config.NoteText[6] == 1) loadImage(NoteColor::W);
		if (config.NoteText[7] == 1) loadImage(NoteColor::K);
		if (config.NoteText[8] == 1) loadImage(NoteColor::F);
	}

	void loadImage(NoteColor noteColor) {
		H_NOTE_TEXT[(int)noteColor] = LoadGraph((rootPath + L"/" + fileName[(int)noteColor]).c_str());
	}
};

