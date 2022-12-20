#include "SoundSet.h"
#include "DxLib.h"
#include <string>

SoundSet::~SoundSet() {//�T�E���h�����
	deleteSound();
}

int SoundSet::getHandle(std::wstring path) {
	if (snd.find(path.c_str()) != snd.end()) {
		//���ɓǂ݂��񂾃T�E���h
	}
	else {
		//���ǂݍ��݃T�E���h
		registSound(path);
	}
	return snd[path];
}

void SoundSet::registSound(std::wstring path) {
	int handle = LoadSoundMem(path.c_str());
	snd[path] = handle; //�T�E���h�p�X�ƃn���h���̓o�^
}

void SoundSet::deleteSound() {
	for (auto itr = snd.begin(); itr != snd.end(); ++itr) {
		DeleteGraph(itr->second);
	}
}