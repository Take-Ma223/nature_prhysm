#pragma once

#include <unordered_map>

class SoundSet {
	std::unordered_map<std::wstring, int> snd;//(サウンドパス,ハンドル)　の辞書
	void registSound(std::wstring);

public:
	~SoundSet();
	int getHandle(std::wstring);

	void deleteSound();
};

