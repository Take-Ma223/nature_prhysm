#pragma once
#include "ImageSet.h"
#include "SoundSet.h"

/**
* �A�Z�b�g�Ǘ��N���X
* �摜�A�T�E���h��o�^���A�����ŊǗ����܂�
* �����摜�Ȃǎg���܂킳���ɂ��̏����ō쐬����摜�̓n���h���o�^�̂ݍs���܂�(registImageHandler)
* Asset�̃f�X�g���N�^���s���ɉ摜�A�T�E���h�͉������܂�
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