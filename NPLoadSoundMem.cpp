#include "NPLoadSoundMem.h"
#include <DxLib.h>

int NPLoadHitSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = 255 * (double)option->op.hitSoundVol / (int)OptionItem::HitSoundVol::Vol_100;
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadFxSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = 255 * (double)option->op.fxSoundVol / (int)OptionItem::FxSoundVol::Vol_100;
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadBgmSoundMem(const wchar_t* FileName, Option* option, int BufferNum, int UnionHandle)
{
	const int volume = 255 * (double)option->op.bgmSoundVol / (int)OptionItem::BgmSoundVol::Vol_100;
	return NPLoadSoundMem(FileName, volume, BufferNum, UnionHandle);
}

int NPLoadSoundMem(const wchar_t* FileName, int volume, int BufferNum, int UnionHandle)
{
	const int handle = LoadSoundMem(FileName, BufferNum, UnionHandle);
	ChangeVolumeSoundMem(volume, handle);
	return handle;
}