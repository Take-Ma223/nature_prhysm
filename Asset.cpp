#include "Asset.h"

Asset::~Asset()
{
	deleteAsset();
}

int Asset::img(std::wstring path)
{
	return asset.getHandle(path);
}

int Asset::snd(std::wstring path)
{
	return soundSet.getHandle(path);
}

void Asset::deleteAsset()
{
	asset.deleteImage();
	soundSet.deleteSound();
}
