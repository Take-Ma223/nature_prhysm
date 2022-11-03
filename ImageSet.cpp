#include "ImageSet.h"
#include "DxLib.h"

ImageSet::~ImageSet(){//グラフィックを解放
	deleteImage();
}

int ImageSet::getHandle(std::wstring path) {
	if (img.find(path.c_str()) != img.end()) {
		//既に読みこんだ画像
	}
	else {
		//初読み込み画像
		registImage(path);
	}
	return img[path];
}

std::vector<int> ImageSet::getHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	if (imgs.find(path.c_str()) != imgs.end()) {
		//既に読みこんだ画像
	}
	else {
		//初読み込み画像
		registImages(path, allNum, XNum, YNum, XSize, YSize);
	}
	return imgs[path];

	return std::vector<int>();
}

void ImageSet::registImage(std::wstring path) {
	int handle = LoadGraph(path.c_str());
	img[path] = handle; //画像パスとハンドルの登録
}

void ImageSet::registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize) {
	std::vector<int> handles(allNum);
	LoadDivGraph(path.c_str(),allNum,XNum,YNum,XSize,YSize,&handles[0]);
	imgs[path] = handles; //画像パスとハンドルの登録
}

void ImageSet::deleteImage() {
	for (auto itr = img.begin(); itr != img.end(); ++itr) {
		DeleteGraph(itr->second);
	}

	for (auto itr = imgs.begin(); itr != imgs.end(); ++itr) {
		for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
			DeleteGraph(*itr2);
		}
	}
}