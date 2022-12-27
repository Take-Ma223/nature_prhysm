#pragma once
#include "ImageSet.h"
#include "SoundSet.h"

/**
* アセット管理クラス
* 画像、サウンドを登録し、辞書で管理します
* 文字画像など使いまわさずにその場限りで作成する画像はハンドル登録のみ行います(registImageHandler)
* Assetのデストラクタ実行時に画像、サウンドは解放されます
*/
class Asset
{
	ImageSet asset;
	SoundSet soundSet;
	void deleteAsset();

public:
	~Asset();
	ImageHandle img(std::wstring path);
	std::vector<ImageHandle> imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
	void registImageHandler(ImageHandle imageHandle);

	int snd(std::wstring path);
};