#include "NPLoadSoundMem.h"
#include <DxLib.h>

int NPLoadHitSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = option->op.hitSoundVol.getVolume();
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadFxSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = option->op.fxSoundVol.getVolume();
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadBgmSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = option->op.bgmSoundVol.getVolume();
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadSoundMem(const wchar_t* FileName, int volume, int BufferNum, int UnionHandle)
{
	const int handle = LoadSoundMem(FileName, BufferNum, UnionHandle);
	ChangeVolumeSoundMem(volume, handle);
	return handle;
}