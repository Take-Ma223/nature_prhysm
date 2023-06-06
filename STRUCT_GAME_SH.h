#ifndef _STRUCT_GAME_SH
#define _STRUCT_GAME_SH

//音声データのハンドル
typedef struct GAME_SH {
	int SH_HIT_N;
	int SH_HIT_L;
	int SH_HIT_S;

	int SH_HIT_R_N;
	int SH_HIT_R_L;
	int SH_HIT_R_S;

	int SH_HIT_G_N;
	int SH_HIT_G_L;
	int SH_HIT_G_S;

	int SH_HIT_B_N;
	int SH_HIT_B_L;
	int SH_HIT_B_S;

	int SH_HIT_K;

	int SH_RELEASE;

	void setVol(int vol) {
		ChangeVolumeSoundMem(vol, SH_HIT_N);
		ChangeVolumeSoundMem(vol, SH_HIT_L);
		ChangeVolumeSoundMem(vol, SH_HIT_S);
		ChangeVolumeSoundMem(vol, SH_HIT_R_N);
		ChangeVolumeSoundMem(vol, SH_HIT_R_L);
		ChangeVolumeSoundMem(vol, SH_HIT_R_S);
		ChangeVolumeSoundMem(vol, SH_HIT_G_N);
		ChangeVolumeSoundMem(vol, SH_HIT_G_L);
		ChangeVolumeSoundMem(vol, SH_HIT_G_S);
		ChangeVolumeSoundMem(vol, SH_HIT_B_N);
		ChangeVolumeSoundMem(vol, SH_HIT_B_L);
		ChangeVolumeSoundMem(vol, SH_HIT_B_S);
		ChangeVolumeSoundMem(vol, SH_HIT_K);
		ChangeVolumeSoundMem(vol, SH_RELEASE);
	}

}GAME_SH;

#endif