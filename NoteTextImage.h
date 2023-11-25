#include <string>
#include <DxLib.h>
#include "STRUCT_NOTE.h"

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

	wstring rootPath;

public:
	int H_NOTE_TEXT[10];

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="path">ノーツテキスト画像へのパス img/notes/XXX</param>
	NoteTextImage(wstring path) {
		rootPath = path;

		loadImage(NoteColor::R);
		loadImage(NoteColor::G);
		loadImage(NoteColor::B);
		loadImage(NoteColor::C);
		loadImage(NoteColor::M);
		loadImage(NoteColor::Y);
		loadImage(NoteColor::W);
		loadImage(NoteColor::K);
		loadImage(NoteColor::F);
	}

	void loadImage(NoteColor noteColor) {
		H_NOTE_TEXT[(int)noteColor] = LoadGraph((rootPath + L"/" + fileName[(int)noteColor]).c_str());
	}
};

