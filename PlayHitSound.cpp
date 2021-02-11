#include"DxLib.h"
#include"PlayHitSound.h"

void PlayHitSound(int judge, int color, int bright, GAME_SH SH) {//叩いた音を鳴らす処理(judge:1でGOOD 2でPERFECT)
#define VOL_MAX 255
#define VOL_MID 255
#define VOL_MIN 255

	if (judge == 1) {//GOOD

		if (color == 1) {
			ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_R_S);
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
		}
		if (color == 2) {
			ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_G_S);
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == 3) {
			ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_B_S);
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 4) {
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_S);
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_S);
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == 5) {
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_S);
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_S);
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 6) {
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_S);
			ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_S);
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 7) {
			ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_R_S);
			ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_G_S);
			ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_B_S);
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 9) {
			ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_S);
			PlaySoundMem(SH.SH_HIT_S, DX_PLAYTYPE_BACK, TRUE);//Fの音出す
		}
	}

	if (judge == 2) {//PERFECT
		if (color == 1) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_R_N);
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_R_L);
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
		}
		if (color == 2) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_G_N);
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_G_L);
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
		}
		if (color == 3) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_B_N);
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_B_L);
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
		}
		if (color == 4) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_N);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_N);
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_L);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_L);
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す									
			}
		}
		if (color == 5) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_N);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_N);
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_G_L);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_L);
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 6) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_N);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_N);
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_R_L);
				ChangeNextPlayVolumeSoundMem(VOL_MID, SH.SH_HIT_B_L);
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 7) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_R_N);
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_G_N);
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_B_N);
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_R_L);
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_G_L);
				ChangeNextPlayVolumeSoundMem(VOL_MIN, SH.SH_HIT_B_L);
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 9) {
			if (bright == 0) {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_N);
				PlaySoundMem(SH.SH_HIT_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
			else {
				ChangeNextPlayVolumeSoundMem(VOL_MAX, SH.SH_HIT_L);
				PlaySoundMem(SH.SH_HIT_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す		
			}
		}
	}

	return;
}