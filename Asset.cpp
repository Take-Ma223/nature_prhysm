#include "Asset.h"

Asset::~Asset()
{
	deleteAsset();
}

int Asset::img(std::wstring path)
{
	return asset.getHandle(path);
}

std::vector<int> Asset::imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	return asset.getHandles(path, allNum, XNum, YNum, XSize, YSize);
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
