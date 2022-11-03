#pragma once

#include <unordered_map>

class ImageSet{
	std::unordered_map<std::wstring, int> img;//(�摜�p�X,�n���h��)�@�̎���
	std::unordered_map<std::wstring, std::vector<int>> imgs;//(�摜�p�X,�n���h���z��)�@�̎���
	void registImage(std::wstring);
	void ImageSet::registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
public:
	~ImageSet();
	int getHandle(std::wstring path);
	std::vector<int> getHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	void deleteImage();
};

