#include "ImageSet.h"
#include "DxLib.h"
#include <string>
#include <stdexcept>

ImageSet::~ImageSet(){//�O���t�B�b�N�����
	deleteImage();
}

ImageHandle ImageSet::getImageHandle(std::wstring path) {
	if (img.find(path.c_str()) != img.end()) {
		//���ɓǂ݂��񂾉摜
	}
	else {
		//���ǂݍ��݉摜
		registImage(path);
	}
	return img[path];
}

std::vector<ImageHandle> ImageSet::getImageHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	if (imgs.find(path.c_str()) != imgs.end()) {
		//���ɓǂ݂��񂾉摜
	}
	else {
		//���ǂݍ��݉摜
		registImages(path, allNum, XNum, YNum, XSize, YSize);
	}
	return imgs[path];
}

void ImageSet::registImage(std::wstring path) {
	int handle = LoadGraph(path.c_str());
	if (handle == -1) {
		throw std::runtime_error("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");
	}

	int x, y;
	GetGraphSize(handle, &x, &y);
	img[path] = ImageHandle(handle, Size(x, y)); //�摜�p�X�ƃn���h���̓o�^
}

void ImageSet::registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize) {
	std::vector<int> handles(allNum);
	if (LoadDivGraph(path.c_str(), allNum, XNum, YNum, XSize, YSize, &handles[0]) == -1) {
		throw std::runtime_error("�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂���");
	}

	std::vector<ImageHandle> singleScreens;
	int x, y;
	for (auto itr = handles.begin(); itr != handles.end(); ++itr) {
		GetGraphSize(*itr, &x, &y);
		singleScreens.push_back(ImageHandle(*itr, Size(x, y)));
	}

	imgs[path] = singleScreens; //�摜�p�X�ƃn���h���̓o�^
}

void ImageSet::registImageHandler(ImageHandle imageHandle)
{
	tmpImageHandles.push_back(imageHandle);
}

void ImageSet::deleteImage() {
	for (auto itr = img.begin(); itr != img.end(); ++itr) {
		DeleteGraph(itr->second.getHandle());
	}

	for (auto itr = imgs.begin(); itr != imgs.end(); ++itr) {
		for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
			DeleteGraph(itr2->getHandle());
		}
	}

	for (auto itr = tmpImageHandles.begin(); itr != tmpImageHandles.end(); ++itr) {
		DeleteGraph(itr->getHandle());
	}
}