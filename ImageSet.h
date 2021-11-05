#pragma once

#include <unordered_map>

class ImageSet{
	std::unordered_map<std::wstring, int> img;//(�摜�p�X,�n���h��)�@�̎���
	void registImage(std::wstring);

public:
	~ImageSet();
	int getHandle(std::wstring);

	void deleteImage();
};

