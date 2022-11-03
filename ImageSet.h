#pragma once

#include <unordered_map>

class ImageSet{
	std::unordered_map<std::wstring, int> img;//(画像パス,ハンドル)　の辞書
	std::unordered_map<std::wstring, std::vector<int>> imgs;//(画像パス,ハンドル配列)　の辞書
	void registImage(std::wstring);
	void ImageSet::registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
public:
	~ImageSet();
	int getHandle(std::wstring path);
	std::vector<int> getHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	void deleteImage();
};

