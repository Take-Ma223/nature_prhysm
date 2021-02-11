#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_SONG.h"
#include"make_result_hash.h"

int ScoreData_Authentication(RESULT before, int song_number, int difficulty, Song *Music) {//スコアデータが不正ではないか認証する(0なら正式,-1なら不正)
	int hash = 0;
	int hash_result = 0;
/*
if (strcmp(before.artist, Music[song_number].artist[difficulty]) != 0) {
return -1;
}
if (strcmp(before.title, Music[song_number].title[difficulty]) != 0) {
return -1;
}
if (strcmp(before.genre, Music[song_number].genre[difficulty]) != 0) {
return -1;
}
*/
	hash = Music[song_number].hash[difficulty - (before.difficulty >= 5) * 4];
	hash_result = make_result_hash(before);

	if (song_number == 12 && difficulty==4) {
		printfDx(L"hash:%d\n", hash);
		printfDx(L"hash_result:%d\n", hash_result);
		printfDx(L"hash:%d\n", before.hash);
		printfDx(L"hash_result:%d\n", before.hash_result);
		ScreenFlip();
		printf("");
	}

	if (before.difficulty != difficulty) {//違う難易度のデータじゃないか



		return -1;
	}
	//printfDx("%d,%d\n", before.difficulty, difficulty);
	//printfDx("hash:%d\n", before.hash != Music[song_number].hash[difficulty - (before.difficulty >= 5) * 4]);
	//ScreenFlip();
	//Sleep(1000);


	if (before.hash != Music[song_number].hash[difficulty - (before.difficulty >= 5) * 4]) {
		return -1;
	}

	if (before.hash_result != make_result_hash(before)) {//リザルトデータの改ざんがあったら
		return -1;
	}




	return 0;
}