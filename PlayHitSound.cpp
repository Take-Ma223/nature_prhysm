#include"DxLib.h"
#include"PlayHitSound.h"

void PlayHitSound(int judge, NoteColor color, int bright, GAME_SH SH) {//叩いた音を鳴らす処理(judge:1でGOOD 2でPERFECT)
	if (judge == 1) {//GOOD

		if (color == NoteColor::R) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
		}
		if (color == NoteColor::G) {
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == NoteColor::B) {
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == NoteColor::Y) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
		}
		if (color == NoteColor::C) {
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == NoteColor::M) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == NoteColor::W) {
			PlaySoundMem(SH.SH_HIT_R_S, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			PlaySoundMem(SH.SH_HIT_G_S, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			PlaySoundMem(SH.SH_HIT_B_S, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
		}
		if (color == NoteColor::F) {
			PlaySoundMem(SH.SH_HIT_S, DX_PLAYTYPE_BACK, TRUE);//Fの音出す
		}
	}

	if (judge == 2) {//PERFECT
		if (color == NoteColor::R) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
			}
		}
		if (color == NoteColor::G) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
		}
		if (color == NoteColor::B) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
		}
		if (color == NoteColor::Y) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す									
			}
		}
		if (color == NoteColor::C) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);//Gの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == NoteColor::M) {
			if (bright == 0) {
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);//Bの音出す
			}
			else {
				PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);//Rの音出す
				PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);//Bの音出す									
			}
		}
		if (color == NoteColor::W) {
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
		if (color == NoteColor::F) {
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