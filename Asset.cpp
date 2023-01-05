#include "Asset.h"

Asset::~Asset()
{
	deleteAsset();
}

ImageHandle Asset::img(std::wstring path)
{
	return asset.getImageHandle(path);
}

std::vector<ImageHandle> Asset::imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	return asset.getImageHandles(path, allNum, XNum, YNum, XSize, YSize);
}

void Asset::registImageHandler(ImageHandle imageHandle)
{
	asset.registImageHandler(imageHandle);
}

int Asset::snd(std::wstring path)
{
	return soundSet.getHandle(path);
}

FontHandle Asset::font(FontInfo fontInfo)
{
	return fontSet.getFontHandle(fontInfo);
}

void Asset::deleteAsset()
{
	asset.deleteImage();
	soundSet.deleteSound();
}
