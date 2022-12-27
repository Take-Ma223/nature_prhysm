#pragma once

#include <unordered_map>
#include <string>
#include "ImageHandle.h"

class ImageSet{
	std::unordered_map<std::wstring, ImageHandle> img;//(�摜�p�X,�n���h�����)�@�̎���
	std::unordered_map<std::wstring, std::vector<ImageHandle>> imgs;//(�摜�p�X,�n���h�����z��)�@�̎���
	void registImage(std::wstring);
	void registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
public:
	~ImageSet();
	ImageHandle getSingleScreen(std::wstring path);
	std::vector<ImageHandle> getSingleScreens(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	void deleteImage();
};

