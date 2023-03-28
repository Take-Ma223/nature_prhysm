#include "Asset.h"

Asset::~Asset()
{
	deleteAsset();
}

ImageHandle Asset::img(std::wstring path)
{
	return imageSet.getImageHandle(path);
}

std::vector<ImageHandle> Asset::imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize)
{
	return imageSet.getImageHandles(path, allNum, XNum, YNum, XSize, YSize);
}

ImageHandle Asset::imgFromMem(std::wstring tag, ImageHandle imageHandle)
{
	imageSet.registImageHandleFromMem(tag, imageHandle);
	return imgFromMem(tag);
}

ImageHandle Asset::imgFromMem(std::wstring tag)
{
	return imageSet.getImageHandleFromMem(tag);
}

void Asset::deleteImgFromMem(std::wstring tag)
{
	imageSet.deleteImageHandleFromMem(tag);
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
	imageSet.deleteImage();
	soundSet.deleteSound();
	fontSet.deleteFont();
}
