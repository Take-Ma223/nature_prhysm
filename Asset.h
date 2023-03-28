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
	/// �⏕�L�����u�ɑ��݂���摜�n���h���̎擾
	/// </summary>
	/// <param name="path">�摜�p�X</param>
	/// <returns>�摜�n���h��</returns>
	ImageHandle img(std::wstring path);


	/// <summary>
	/// �⏕�L�����u�ɑ��݂��镪���摜�̉摜�n���h���z��𓾂�
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
	/// �������ɑ��݂���摜�n���h���̎擾
	/// �ŏ��̎擾�͕K�����̃��\�b�h���g���ĉ摜�n���h����o�^���Ă�������
	/// </summary>
	/// <param name="tag">�������ɑ��݂���摜�n���h���̃^�O</param>
	/// <param name="imageHandle">�������ɑ��݂���摜�n���h��</param>
	/// <returns><�摜�n���h��/returns>
	ImageHandle imgFromMem(std::wstring tag, ImageHandle imageHandle);
	
	/// <summary>
	/// �������ɑ��݂���摜�n���h���̎擾
	/// </summary>
	/// <param name="tag">�擾����摜�n���h���̃^�O</param>
	/// <returns>�摜�n���h��</returns>
	ImageHandle imgFromMem(std::wstring tag);

	/// <summary>
	/// �������ɑ��݂���摜�n���h���̍폜
	/// </summary>
	/// <param name="tag">�폜����摜�n���h���̃^�O</param>
	void deleteImgFromMem(std::wstring tag);

	int snd(std::wstring path);

	/// <summary>
	/// �t�H���g�n���h���𓾂�
	/// </summary>
	/// <param name="fontInfo">�t�H���g���</param>
	/// <returns>�t�H���g�n���h��</returns>
	FontHandle font(FontInfo fontInfo);
};