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
	int img(std::wstring path);
	int snd(std::wstring path);
};
