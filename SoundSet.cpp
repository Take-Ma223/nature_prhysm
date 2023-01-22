#include "SoundSet.h"
#include "DxLib.h"
#include <string>
#include <stdexcept>

SoundSet::~SoundSet() {//サウンドを解放
	deleteSound();
}

int SoundSet::getHandle(std::wstring path) {
	if (snd.find(path.c_str()) != snd.end()) {
		//既に読みこんだサウンド
	}
	else {
		//初読み込みサウンド
		registSound(path);
	}
	return snd[path];
}

void SoundSet::registSound(std::wstring path) {
	int handle = LoadSoundMem(path.c_str());
	if (handle == -1) {
		throw std::runtime_error("ファイルの読み込みに失敗しました");
	}
	snd[path] = handle; //サウンドパスとハンドルの登録
}

void SoundSet::deleteSound() {
	for (auto itr = snd.begin(); itr != snd.end(); ++itr) {
		DeleteGraph(itr->second);
	}
}