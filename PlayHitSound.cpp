#include"DxLib.h"
#include"PlayHitSound.h"

void PlayHitSound(int judge, int color, int bright, GAME_SH SH) {//叩いた音を鳴らす処理(judge:1でGOOD 2でPERFECT)
	if (judge == 1) {//GOOD

		if (color == 1) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
		}
		if (color == 2) {
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == 3) {
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 4) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == 5) {
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 6) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 7) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == 9) {
			PlaySoundMem(SH.SH_HIT_S, DX_PLAYTYPE_BACK, TRUE);//Fの音出す
		}
	}

	if (judge == 2) {//PERFECT
		if (color == 1) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
		}
		if (color == 2) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
		}
		if (color == 3) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
		}
		if (color == 4) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す									
			}
		}
		if (color == 5) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 6) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 7) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == 9) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す		
			}
		}
	}

	return;
}