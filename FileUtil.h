#pragma once
#include "DxLib.h"
#include "vector"
#include "string"
#include "DxLibUtil.h"
#include "ShowErrorName.h"

using namespace std;

struct FileUtil {
	/// <summary>
	/// Path�Ɋ܂܂��t�H���_���X�g���擾
	/// </summary>
	/// <param name="path"></param>
	/// <returns>�t�H���_���X�g</returns>
	static vector<wstring> getFolderList(wstring path) {
		vector<wstring> pathList;

		HANDLE hFind_HitSounds = 0;//������p�n���h��
		WIN32_FIND_DATA lp;

		hFind_HitSounds = FindFirstFile((path + L"/*").c_str(), &lp);//�t�H���_�̍ŏ��̒T��
		do {
			if (ProcessMessage() != 0) {
				dxLibFinishProcess();
			}
			bool is_directory = lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && is_directory) {//���g�Ɛe�ȊO�̃f�B���N�g����������
				pathList.push_back(lp.cFileName);
			}
		} while (FindNextFile(hFind_HitSounds, &lp));//����������Ȃ�������0�ɂȂ胋�[�v�𔲂���
		if (FindClose(hFind_HitSounds) == 0) {
			ShowError(L"�t�H���_������܂���B");
			dxLibFinishProcess();
		}

		return pathList;
	}
};