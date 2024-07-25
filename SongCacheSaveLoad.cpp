#include"SongCacheSaveLoad.h"
#include"STRUCT_Song.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_OP.h"


void writeCacheToMusic(SongCache* Cache, Song* Music, int song_number, int difficulty) {
	int i = 0;
	for (i = Speed::AVERAGE; i <= Speed::MAX; i++) {
		Music[song_number].speed_list[difficulty][i] = Cache->bpm_suggested[i];
	}

	Music[song_number].total_note[difficulty] = Cache->total_note;

	for (i = 0; i < 9; i++) {
		Music[song_number].LocalNotesAmount[RainbowMode::General][difficulty][i] = Cache->LocalNotesAmount[RainbowMode::General][i];
		Music[song_number].ColorNotesAmount[RainbowMode::General][difficulty][i] = Cache->ColorNotesAmount[RainbowMode::General][i];
		Music[song_number].LocalNotesAmount[RainbowMode::Rainbow][difficulty][i] = Cache->LocalNotesAmount[RainbowMode::Rainbow][i];
		Music[song_number].ColorNotesAmount[RainbowMode::Rainbow][difficulty][i] = Cache->ColorNotesAmount[RainbowMode::Rainbow][i];
	}
	for (i = 0; i < 2; i++) {
		Music[song_number].global[i][difficulty] = Cache->global[i];//�~����(����)
		Music[song_number].local[i][difficulty] = Cache->local[i];//�Ǐ��~����(0~1�ɐ��K�����ꂽ�F�d�݂Â��z�u�^�C�~���O�̕W���΍� �������قǋǏ���)
		Music[song_number].color[i][difficulty] = Cache->color[i];//�F�ω��x
		Music[song_number].longNote[i][difficulty] = Cache->longNote[i];//LN��
		Music[song_number].unstability[i][difficulty] = Cache->unstability[i];//��C�s����x(�\�t�����x)
		Music[song_number].chain[i][difficulty] = Cache->chain[i];//�c�A�x

		Music[song_number].maxChords[i][difficulty] = Cache->maxChords[i];//�ő哯��������

	}

	return;
}

void writeMusicToCache(SongCache *Cache, Song* Music, int song_number, int difficulty) {
	int i = 0;
	for (i = Speed::AVERAGE; i <= Speed::MAX; i++) {
		Cache->bpm_suggested[i] = Music[song_number].speed_list[difficulty][i];
	}
	Cache->total_note = Music[song_number].total_note[difficulty];

	for (i = 0; i < 9; i++) {
		Cache->LocalNotesAmount[RainbowMode::General][i] = Music[song_number].LocalNotesAmount[RainbowMode::General][difficulty][i];
		Cache->ColorNotesAmount[RainbowMode::General][i] = Music[song_number].ColorNotesAmount[RainbowMode::General][difficulty][i];
		Cache->LocalNotesAmount[RainbowMode::Rainbow][i] = Music[song_number].LocalNotesAmount[RainbowMode::Rainbow][difficulty][i];
		Cache->ColorNotesAmount[RainbowMode::Rainbow][i] = Music[song_number].ColorNotesAmount[RainbowMode::Rainbow][difficulty][i];
	}
	for (i = 0; i < 2; i++) {
		Cache->global[i] = Music[song_number].global[i][difficulty];//�~����(����)
		Cache->local[i] = Music[song_number].local[i][difficulty];//�Ǐ��~����(0~1�ɐ��K�����ꂽ�F�d�݂Â��z�u�^�C�~���O�̕W���΍� �������قǋǏ���)
		Cache->color[i] = Music[song_number].color[i][difficulty];//�F�ω��x
		Cache->longNote[i] = Music[song_number].longNote[i][difficulty];//LN��
		Cache->unstability[i] = Music[song_number].unstability[i][difficulty];//��C�s����x(�\�t�����x)
		Cache->chain[i] = Music[song_number].chain[i][difficulty];//�c�A�x

		Cache->maxChords[i] = Music[song_number].maxChords[i][difficulty];//�ő哯��������
	}
	
	return;
}

void SaveSongCache(SongCache Cache, Song *Music, int song_number, int difficulty) {
	FILE* fp = 0;
	errno_t error = 0;
	wchar_t str[256];

	sprintfDx(str, L"cache/%s", Music[song_number].KindFolder);
	CreateDirectory(str, NULL);//�L���b�V���ۑ��p�̎�ރf�B���N�g��(official)�쐬

	CreateDirectory(Music[song_number].CacheFolder, NULL);//�L���b�V���ۑ��p�̋ȃf�B���N�g���쐬

	sprintfDx(str, L"%s/%d.dat", Music[song_number].CacheFolder, difficulty);

	error = _wfopen_s(&fp, str, L"wb");
	if (error != 0) {
		printfDx(L"%s�̃I�[�v���Ɏ��s���܂���.\n", str);
		printfDx(L"�G���[�ԍ�:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&Cache, sizeof(Cache), 1, fp);
	fclose(fp);
	return;
}


int LoadSongCache(SongCache* Cache, Song* Music, int song_number, int difficulty) {

	//Cache������
	Cache->writeTime.dwLowDateTime = 0;
	Cache->writeTime.dwHighDateTime = 0;
	int i = 0;
	Cache->total_note = 0;

	for (i = 0; i < 9; i++) {
		Cache->LocalNotesAmount[RainbowMode::General][i] = 0;
		Cache->ColorNotesAmount[RainbowMode::General][i] = 0;
		Cache->LocalNotesAmount[RainbowMode::Rainbow][i] = 0;
		Cache->ColorNotesAmount[RainbowMode::Rainbow][i] = 0;
	}
	for (i = 0; i < 2; i++) {
		Cache->global[i] = 0;//�~����(����)
		Cache->local[i] = 0;//�Ǐ��~����(0~1�ɐ��K�����ꂽ�F�d�݂Â��z�u�^�C�~���O�̕W���΍� �������قǋǏ���)
		Cache->color[i] = 0;//�F�ω��x
		Cache->longNote[i] = 0;//LN��
		Cache->unstability[i] = 0;//��C�s����x(�\�t�����x)
		Cache->chain[i] = 0;//�c�A�x

		Cache->maxChords[i] = 0;//�ő哯��������
	}



	FILE* fp = 0;
	errno_t error = 0;

	wchar_t str[256];
	sprintfDx(str, L"%s/%d.dat", Music[song_number].CacheFolder, difficulty);

	error = _wfopen_s(&fp, str, L"rb");
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
		return 1;
	}
	else {
		fread(Cache, sizeof(*Cache), 1, fp);//�ǂݍ���
		fclose(fp);
		return 0;
	}

	return 0;
}