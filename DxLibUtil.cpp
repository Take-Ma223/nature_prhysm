#include "DxLibUtil.h"
#include "DxLib.h"
#include <EffekseerForDXLib.h>

void dxLibFinishProcess()
{
	InitSoundMem();
	Effkseer_End();
	DxLib_End();
	exit(0);
}
