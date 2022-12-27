#pragma once

#include <unordered_map>
#include <string>
#include "ImageHandle.h"

class ImageSet{
	std::unordered_map<std::wstring, ImageHandle> img;//(画像パス,ハンドル情報)　の辞書
	std::unordered_map<std::wstring, std::vector<ImageHandle>> imgs;//(画像パス,ハンドル情報配列)　の辞書
	void registImage(std::wstring);
	void registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
public:
	~ImageSet();
	ImageHandle getSingleScreen(std::wstring path);
	std::vector<ImageHandle> getSingleScreens(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	void deleteImage();
};

