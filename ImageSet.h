#pragma once

#include <unordered_map>

class ImageSet{
	std::unordered_map<std::wstring, int> img;//(画像パス,ハンドル)　の辞書
	void registImage(std::wstring);

public:
	~ImageSet();
	int getHandle(std::wstring);

	void deleteImage();
};

