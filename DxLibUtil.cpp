#include "DxLibUtil.h"
#include "DxLib.h"
#include <EffekseerForDXLib.h>
#include "AutoDifficultyPrediction.h"

void dxLibFinishProcess()
{
	InitSoundMem();
	Effkseer_End();
	DxLib_End();

	AutoDifficultyPrediction adp;
	adp.killServer();

	exit(0);
}
