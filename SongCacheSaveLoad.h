#include"STRUCT_Song.h"
#include"STRUCT_OP.h"
#include<sys/stat.h>
#include<windows.h>

#ifndef _SONGCACHESAVELOAD
#define _SONGCACHESAVELOAD

typedef struct SongCache {//���ʃL���b�V���\����
	FILETIME writeTime;//nps�t�@�C���̍X�V����

	short bpm_suggested;//��Ă����n�C�X�s�ݒ�Ɉ�ԗǂ�BPM
	unsigned short total_note;//���m�[�g��

	short LocalNotesAmount[9] = { 0,0,0,0,0,0,0,0,0 };//���x�ω��O���t
	short ColorNotesAmount[9] = { 0,0,0,0,0,0,0,0,0 };//RGBCMYWBF���ꂼ��̐�

	short global[2];//�~����(����)
	short local[2];//�Ǐ��~����(0~1�ɐ��K�����ꂽ�F�d�݂Â��z�u�^�C�~���O�̕W���΍� �������قǋǏ���)
	short color[2];//�F�ω��x
	short longNote[2];//LN��
	short unstability[2];//��C�s����x(�\�t�����x)
	short chain[2];//�c�A�x

}SongCache;

extern void writeCacheToMusic(SongCache* Cache, Song* Music, int song_number, int difficulty);
extern void writeMusicToCache(SongCache* Cache, Song* Music, int song_number, int difficulty);
extern void SaveSongCache(SongCache Cache, Song* Music, int song_number, int difficulty);
extern int LoadSongCache(SongCache* Cache, Song* Music, int song_number, int difficulty);



#endif