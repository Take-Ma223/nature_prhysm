#pragma once
#include "DxLib.h"
#include "vector"
#include "string"
#include "DxLibUtil.h"
#include "ShowErrorName.h"

using namespace std;

struct FileUtil {
	/// <summary>
	/// Pathに含まれるフォルダリストを取得
	/// </summary>
	/// <param name="path"></param>
	/// <returns>フォルダリスト</returns>
	static vector<wstring> getFolderList(wstring path) {
		vector<wstring> pathList;

		HANDLE hFind_HitSounds = 0;//見つける用ハンドル
		WIN32_FIND_DATA lp;

		hFind_HitSounds = FindFirstFile((path + L"/*").c_str(), &lp);//フォルダの最初の探索
		do {
			if (ProcessMessage() != 0) {
				dxLibFinishProcess();
			}
			bool is_directory = lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && is_directory) {//自身と親以外のディレクトリを見つけた
				pathList.push_back(lp.cFileName);
			}
		} while (FindNextFile(hFind_HitSounds, &lp));//何も見つからなかったら0になりループを抜ける
		if (FindClose(hFind_HitSounds) == 0) {
			ShowError(L"フォルダがありません。");
			dxLibFinishProcess();
		}

		return pathList;
	}
};