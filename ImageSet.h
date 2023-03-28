#pragma once

#include <unordered_map>
#include <string>
#include "ImageHandle.h"

/// <summary>
/// �摜�A�Z�b�g�Ǘ��N���X
/// ���̃N���X�œǂݍ��񂾉摜�͉�ʏI�����ɔj������܂�
/// </summary>
class ImageSet{
public:
	~ImageSet();
	
	/// <summary>
	/// �⏕�L�����u�ɑ��݂���摜�n���h���̎擾
	/// </summary>
	/// <param name="path">�摜�p�X</param>
	/// <returns>�摜�n���h��</returns>
	ImageHandle getImageHandle(std::wstring path);

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
	std::vector<ImageHandle> getImageHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
	
	/// <summary>
	/// �������ɑ��݂���摜�n���h����o�^����
	/// ���̃��\�b�h�œo�^���邱�ƂŎ����ŉ������܂�
	/// </summary>
	/// <param name="tag">�������ɑ��݂���摜�n���h���̃^�O</param>
	/// <param name="imageHandle">�������ɑ��݂���摜�n���h��</param>
	void registImageHandleFromMem(std::wstring tag, ImageHandle imageHandle);

	/// <summary>
	/// �������ɑ��݂���摜�n���h���̎擾
	/// </summary>
	/// <param name="tag">�擾����摜�n���h���̃^�O</param>
	/// <returns>�摜�n���h��</returns>
	ImageHandle getImageHandleFromMem(std::wstring tag);

	/// <summary>
	/// �������ɑ��݂���摜�n���h���̍폜
	/// </summary>
	/// <param name="tag">�폜����摜�n���h���̃^�O</param>
	void deleteImageHandleFromMem(std::wstring tag);

	/// <summary>
	/// �ǂݍ��񂾑S�Ẳ摜�n���h���̍폜
	/// </summary>
	void deleteImage();
private:

	std::unordered_map<std::wstring, ImageHandle> img;//(�摜�p�X,�n���h�����)�@�̎���
	std::unordered_map<std::wstring, std::vector<ImageHandle>> imgs;//(�摜�p�X,�n���h�����z��)�@�̎���
	std::unordered_map<std::wstring, ImageHandle> imgFromMem;//(�摜�^�O,�n���h�����)�̎��� ����������ǂݍ��މ摜�p

	void registImage(std::wstring);
	void registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);


};

