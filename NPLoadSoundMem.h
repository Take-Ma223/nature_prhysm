#pragma once
#include "STRUCT_OP.h"


/// <summary>
/// ヒット音読み込み
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadHitSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// 効果音読み込み
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadFxSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// BGM読み込み
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadBgmSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// 音量設定付きLoadSoundMem
/// </summary>
int NPLoadSoundMem(const wchar_t* FileName, int volume, int BufferNum, int UnionHandle);
