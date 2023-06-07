#pragma once
#include "STRUCT_OP.h"


/// <summary>
/// �q�b�g���ǂݍ���
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadHitSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// ���ʉ��ǂݍ���
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadFxSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// BGM�ǂݍ���
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadBgmSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// ���ʐݒ�t��LoadSoundMem
/// </summary>
int NPLoadSoundMem(const wchar_t* FileName, int volume, int BufferNum, int UnionHandle);
