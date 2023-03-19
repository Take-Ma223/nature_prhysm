#include "ImageSet.h"
#include "DxLib.h"
#include <string>
#include <stdexcept>

ImageSet::~ImageSet(){//グラフィックを解放
	deleteImage();
}

ImageHandle ImageSet::getImageHandle(std::wstring path) {
	if (img.find(path.c_str()) != img.end()) {
		//既に読みこんだ画像
	}
	else {
		//初読み込み画像
		registImage(path);
	}
	return img[path];
}

std::vector<ImageHandle> ImageSet::getImageHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	if (imgs.find(path.c_str()) != imgs.end()) {
		//既に読みこんだ画像
	}
	else {
		//初読み込み画像
		registImages(path, allNum, XNum, YNum, XSize, YSize);
	}
	return imgs[path];
}

void ImageSet::registImage(std::wstring path) {
	int handle = LoadGraph(path.c_str());
	if (handle == -1) {
		throw std::runtime_error("画像ファイルの読み込みに失敗しました");
	}

	int x, y;
	GetGraphSize(handle, &x, &y);
	img[path] = ImageHandle(handle, Size(x, y)); //画像パスとハンドルの登録
}

void ImageSet::registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize) {
	std::vector<int> handles(allNum);
	if (LoadDivGraph(path.c_str(), allNum, XNum, YNum, XSize, YSize, &handles[0]) == -1) {
		throw std::runtime_error("画像ファイルの読み込みに失敗しました");
	}

	std::vector<ImageHandle> singleScreens;
	int x, y;
	for (auto itr = handles.begin(); itr != handles.end(); ++itr) {
		GetGraphSize(*itr, &x, &y);
		singleScreens.push_back(ImageHandle(*itr, Size(x, y)));
	}

	imgs[path] = singleScreens; //画像パスとハンドルの登録
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