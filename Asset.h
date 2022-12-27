#pragma once
#include "ImageSet.h"
#include "SoundSet.h"

//アセット管理クラス
//画像、サウンドを登録し、辞書で管理します
class Asset
{
	ImageSet asset;
	SoundSet soundSet;
	void deleteAsset();

public:
	~Asset();
	ImageHandle img(std::wstring path);
	std::vector<ImageHandle> imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
	int snd(std::wstring path);
};

