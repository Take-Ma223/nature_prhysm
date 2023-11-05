#include <string>
#include <DxLib.h>
#include "STRUCT_NOTE.h"

using namespace std;

#pragma once
class GradationNoteImage
{
	wstring fileName[10] = {
		wstring(L""),
		wstring(L"rl.png"),
		wstring(L"gl.png"),
		wstring(L"bl.png"),
		wstring(L"cl.png"),
		wstring(L"ml.png"),
		wstring(L"yl.png"),
		wstring(L"wl.png"),
		wstring(L"dl.png"),
		wstring(L"fl.png"),
	};

	wstring rootPath;

public:
	int H_LNOTE_GRAD_FADE_OUT[10], H_LNOTE_GRAD_FADE_IN[10];

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="path">ノーツ画像へのパス img/notes/XXX</param>
	GradationNoteImage(wstring path) {
		rootPath = path;
		createFadeOutImage(NoteColor::R);
		createFadeOutImage(NoteColor::G);
		createFadeOutImage(NoteColor::B);
		createFadeOutImage(NoteColor::C);
		createFadeOutImage(NoteColor::M);
		createFadeOutImage(NoteColor::Y);
		createFadeOutImage(NoteColor::W);
		createFadeOutImage(NoteColor::K);
		createFadeOutImage(NoteColor::F);

		createFadeInImage(NoteColor::R);
		createFadeInImage(NoteColor::G);
		createFadeInImage(NoteColor::B);
		createFadeInImage(NoteColor::C);
		createFadeInImage(NoteColor::M);
		createFadeInImage(NoteColor::Y);
		createFadeInImage(NoteColor::W);
		createFadeInImage(NoteColor::K);
		createFadeInImage(NoteColor::F);

	}

	void createFadeInImage(NoteColor noteColor) {
		int softImage = LoadSoftImage((rootPath + L"/" + fileName[(int)noteColor]).c_str());

		for (int y = 0; y < 256; y++) {//透過グラデーション加工
			for (int x = 0; x < 256; x++) {
				int r = 0, g = 0, b = 0, a = 0;
				GetPixelSoftImage(softImage, x, y, &r, &g, &b, &a);

				double alphaRatio = ratioFunc(255 - y);
				if (a != 0)DrawPixelSoftImage(softImage, x, y, r, g, b, alphaRatio * a);

			}
		}

		H_LNOTE_GRAD_FADE_IN[(int)noteColor] = CreateGraphFromSoftImage(softImage);
		DeleteSoftImage(softImage);
	}

	void createFadeOutImage(NoteColor noteColor) {
		int softImage = LoadSoftImage((rootPath + L"/" + fileName[(int)noteColor]).c_str());

		for (int y = 0; y < 256; y++) {//透過グラデーション加工
			for (int x = 0; x < 256; x++) {
				int r = 0, g = 0, b = 0, a = 0;
				GetPixelSoftImage(softImage, x, y, &r, &g, &b, &a);

				double alphaRatio = 1 - ratioFunc(255 - y);
				if (a != 0)DrawPixelSoftImage(softImage, x, y, r, g, b, alphaRatio * a);

			}
		}

		H_LNOTE_GRAD_FADE_OUT[(int)noteColor] = CreateGraphFromSoftImage(softImage);
		DeleteSoftImage(softImage);
	}

	/// <summary>
	/// 0~1を返す
	/// </summary>
	/// <param name="input"></param>
	/// <returns></returns>
	double ratioFunc(double input) {
		double pi = 3.14159265;
		return pow(1.0 - (double)cos((pi / 2) * (input / 255)), 1.5);
	}

};

