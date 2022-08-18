﻿#include"folder_insert.h"

void folder_insert(LIST *song_folder, int song_number, int difficulty, Song *Music) {//難易度単位でのフォルダへの挿入
	int level = 0;
	int folder = 0;

	if (Music[song_number].exist[difficulty] == 1) {
		level = (Music[song_number].level[difficulty] / 5);//5~100%を1~20に変換
		folder = level + FolderIndex::DIFFICULTY_5 - 1;//1~20を難易度別フォルダの番号に変換(0~6はALL SONGS~NOT SEASONALで使われてる)

		song_folder->folder[folder][song_folder->folder_c[folder]].song_number = song_number;
		song_folder->folder[folder][song_folder->folder_c[folder]].difficulty = difficulty;
		song_folder->folder_c[folder]++;

		//ALL_DIFFICULTYフォルダ処理
		song_folder->folder[FolderIndex::ALL_DIFFICULTY][song_folder->folder_c[FolderIndex::ALL_DIFFICULTY]].song_number = song_number;
		song_folder->folder[FolderIndex::ALL_DIFFICULTY][song_folder->folder_c[FolderIndex::ALL_DIFFICULTY]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::ALL_DIFFICULTY]++;

	}

	return;
}


void folder_insert_season(LIST *song_folder, int song_number, Song *Music) {//曲単位でのフォルダへの挿入　//ALL_SONGSと季節曲の分
	int level = 0;

	int exist = 0;//対象の譜面があったかどうかのフラグ

	exist = (Music[song_number].exist[1] == 1 && Music[song_number].season[1] == 1)
		|| (Music[song_number].exist[2] == 1 && Music[song_number].season[2] == 1)
		|| (Music[song_number].exist[3] == 1 && Music[song_number].season[3] == 1)
		|| (Music[song_number].exist[4] == 1 && Music[song_number].season[4] == 1);

	if (exist == 1) {//SPRING
		song_folder->folder[FolderIndex::SPRING][song_folder->folder_c[FolderIndex::SPRING]].song_number = song_number;
		//song_folder->folder[1][song_folder->folder_c[1]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::SPRING]++;
	}

	exist = (Music[song_number].exist[1] == 1 && Music[song_number].season[1] == 2)
		|| (Music[song_number].exist[2] == 1 && Music[song_number].season[2] == 2)
		|| (Music[song_number].exist[3] == 1 && Music[song_number].season[3] == 2)
		|| (Music[song_number].exist[4] == 1 && Music[song_number].season[4] == 2);

	if (exist == 1) {//SUMMER
		song_folder->folder[FolderIndex::SUMMER][song_folder->folder_c[FolderIndex::SUMMER]].song_number = song_number;
		//song_folder->folder[2][song_folder->folder_c[2]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::SUMMER]++;
	}

	exist = (Music[song_number].exist[1] == 1 && Music[song_number].season[1] == 3)
		|| (Music[song_number].exist[2] == 1 && Music[song_number].season[2] == 3)
		|| (Music[song_number].exist[3] == 1 && Music[song_number].season[3] == 3)
		|| (Music[song_number].exist[4] == 1 && Music[song_number].season[4] == 3);

	if (exist == 1) {//AUTUMN
		song_folder->folder[FolderIndex::AUTUMN][song_folder->folder_c[FolderIndex::AUTUMN]].song_number = song_number;
		//song_folder->folder[3][song_folder->folder_c[3]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::AUTUMN]++;
	}

	exist = (Music[song_number].exist[1] == 1 && Music[song_number].season[1] == 4)
		|| (Music[song_number].exist[2] == 1 && Music[song_number].season[2] == 4)
		|| (Music[song_number].exist[3] == 1 && Music[song_number].season[3] == 4)
		|| (Music[song_number].exist[4] == 1 && Music[song_number].season[4] == 4);

	if (exist == 1) {//WINTER
		song_folder->folder[FolderIndex::WINTER][song_folder->folder_c[FolderIndex::WINTER]].song_number = song_number;
		//song_folder->folder[4][song_folder->folder_c[4]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::WINTER]++;
	}

	exist = (Music[song_number].exist[1] == 1 && Music[song_number].season[1] == 0)
		|| (Music[song_number].exist[2] == 1 && Music[song_number].season[2] == 0)
		|| (Music[song_number].exist[3] == 1 && Music[song_number].season[3] == 0)
		|| (Music[song_number].exist[4] == 1 && Music[song_number].season[4] == 0);

	if (exist == 1) {//NOT_SEASONAL
		song_folder->folder[FolderIndex::NOT_SEASONAL][song_folder->folder_c[FolderIndex::NOT_SEASONAL]].song_number = song_number;
		//song_folder->folder[5][song_folder->folder_c[5]].difficulty = difficulty;
		song_folder->folder_c[FolderIndex::NOT_SEASONAL]++;
	}


	song_folder->folder[FolderIndex::ALL_SONGS][song_folder->folder_c[FolderIndex::ALL_SONGS]].song_number = song_number;//ALL_SONGSの分
	song_folder->folder_c[FolderIndex::ALL_SONGS]++;

	return;
}

void folder_insert_radar(LIST* song_folder, int song_number, int difficulty, Song* Music)//難易度単位で気象レーダーフォルダへの挿入
{
	int level = 0;
	int folder = 0;

	auto insert = [&](FolderIndex ind) {
		song_folder->folder[ind][song_folder->folder_c[ind]].song_number = song_number;
		song_folder->folder[ind][song_folder->folder_c[ind]].difficulty = difficulty;
		song_folder->folder_c[ind]++;
	};

	auto insertIfTop = [&](Radar radar, FolderIndex ind) {
		if (Music[song_number].isTop(radar, difficulty)) {
			insert(ind);
		}
	};

	if (Music[song_number].exist[difficulty] != 1) { return; }

	insertIfTop(Radar::Global, FolderIndex::GLOBAL);
	insertIfTop(Radar::Local, FolderIndex::LOCAL);
	insertIfTop(Radar::Chain, FolderIndex::CHAIN);
	insertIfTop(Radar::Unstability, FolderIndex::UNSTABILITY);
	insertIfTop(Radar::LongNote, FolderIndex::STREAK);
	insertIfTop(Radar::Color, FolderIndex::COLOR);

	return;
}
