#pragma once

/**
* ���C�����[�v�ŏ������ĂԂ��Ƃ��ł���N���X�̃C���^�[�t�F�[�X
* 
**/
struct Processable
{
public:
	virtual ~Processable() {};
	virtual void process() = 0;
};

