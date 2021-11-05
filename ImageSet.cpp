#include "ImageSet.h"
#include "DxLib.h"

ImageSet::~ImageSet(){//�O���t�B�b�N�����
	deleteImage();
}

int ImageSet::getHandle(std::wstring path) {
	if (img.find(path.c_str()) != img.end()) {
		//���ɓǂ݂��񂾉摜
	}
	else {
		//���ǂݍ��݉摜
		registImage(path);
	}
	return img[path];
}

void ImageSet::registImage(std::wstring path) {
	int handle = LoadGraph(path.c_str());
	img[path] = handle; //�摜�p�X�ƃn���h���̓o�^
}

void ImageSet::deleteImage() {
	for (auto itr = img.begin(); itr != img.end(); ++itr) {
		DeleteGraph(itr->second);
	}
}