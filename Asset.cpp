#include "Asset.h"

Asset::~Asset()
{
	deleteAsset();
}

ImageHandle Asset::img(std::wstring path)
{
	return asset.getSingleScreen(path);
}

std::vector<ImageHandle> Asset::imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	return asset.getSingleScreens(path, allNum, XNum, YNum, XSize, YSize);
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
