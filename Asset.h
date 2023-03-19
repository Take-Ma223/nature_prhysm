#pragma once
#include "ImageSet.h"
#include "SoundSet.h"
#include "FontSet.h"

/**
* �A�Z�b�g�Ǘ��N���X
* �摜�A�T�E���h��o�^���A�����ŊǗ����܂�
* �����摜�Ȃǎg���܂킳���ɂ��̏����ō쐬����摜�̓n���h���o�^�̂ݍs���܂�(registImageHandler)
* Asset�̃f�X�g���N�^���s���ɉ摜�A�T�E���h�͉������܂�
*/
class Asset
{
	ImageSet imageSet;
	SoundSet soundSet;
	FontSet fontSet;
	void deleteAsset();

public:
	~Asset();

	/// <summary>
	/// �摜1�̃n���h���𓾂�
	/// </summary>
	/// <param name="path">�摜�p�X</param>
	/// <returns>�摜�n���h��</returns>
	ImageHandle img(std::wstring path);


	/// <summary>
	/// �����摜�̃n���h���𓾂�
	/// </summary>
	/// <param name="path">�摜�p�X</param>
	/// <param name="allNum">�����摜�̌�</param>
	/// <param name="XNum">���̌�</param>
	/// <param name="YNum">�c�̌�</param>
	/// <param name="XSize">X�T�C�Y</param>
	/// <param name="YSize">Y�T�C�Y</param>
	/// <returns>�摜�n���h���z��</returns>
	std::vector<ImageHandle> imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	/// <summary>
	/// �摜�n���h����o�^����
	/// </summary>
	/// <param name="imageHandle">�摜�n���h��</param>
	void registImageHandler(ImageHandle imageHandle);


	int snd(std::wstring path);

	/// <summary>
	/// �t�H���g�n���h���𓾂�
	/// </summary>
	/// <param name="fontInfo">�t�H���g���</param>
	/// <returns>�t�H���g�n���h��</returns>
	FontHandle font(FontInfo fontInfo);
};