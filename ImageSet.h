#pragma once

#include <unordered_map>
#include <string>
#include "ImageHandle.h"

class ImageSet{
	std::unordered_map<std::wstring, ImageHandle> img;//(画像パス,ハンドル情報)　の辞書
	std::unordered_map<std::wstring, std::vector<ImageHandle>> imgs;//(画像パス,ハンドル情報配列)　の辞書
	void registImage(std::wstring);
	void registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	std::vector<ImageHandle> tmpImageHandles;//ハンドル情報の配列(文字画像など使いまわさずその場限りで使う画像用)
public:
	~ImageSet();
	ImageHandle getImageHandle(std::wstring path);
	std::vector<ImageHandle> getImageHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
	void registImageHandler(ImageHandle imageHandle);

	void deleteImage();
};

