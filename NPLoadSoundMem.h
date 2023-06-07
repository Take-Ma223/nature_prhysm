#pragma once
#include "STRUCT_OP.h"


/// <summary>
/// ƒqƒbƒg‰¹“Ç‚İ‚İ
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadHitSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// Œø‰Ê‰¹“Ç‚İ‚İ
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadFxSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// BGM“Ç‚İ‚İ
/// </summary>
/// <param name="FileName"></param>
/// <param name="option"></param>
/// <param name="BufferNum"></param>
/// <param name="UnionHandle"></param>
/// <returns></returns>
extern int NPLoadBgmSoundMem(const wchar_t* FileName, Option* option, int BufferNum = 3, int UnionHandle = -1);

/// <summary>
/// ‰¹—Êİ’è•t‚«LoadSoundMem
/// </summary>
int NPLoadSoundMem(const wchar_t* FileName, int volume, int BufferNum, int UnionHandle);
