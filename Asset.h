#pragma once
#include "ImageSet.h"
#include "SoundSet.h"

//�A�Z�b�g�Ǘ��N���X
//�摜�A�T�E���h��o�^���A�����ŊǗ����܂�
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

