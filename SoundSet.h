#pragma once

#include <unordered_map>

class SoundSet {
	std::unordered_map<std::wstring, int> snd;//(�T�E���h�p�X,�n���h��)�@�̎���
	void registSound(std::wstring);

public:
	~SoundSet();
	int getHandle(std::wstring);

	void deleteSound();
};

