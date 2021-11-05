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

void ImageSet::registImage(std::wstring path) {
	int handle = LoadGraph(path.c_str());
	img[path] = handle; //画像パスとハンドルの登録
}

void ImageSet::deleteImage() {
	for (auto itr = img.begin(); itr != img.end(); ++itr) {
		DeleteGraph(itr->second);
	}
}