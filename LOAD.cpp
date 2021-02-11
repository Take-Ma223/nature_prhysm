﻿#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include<windows.h>
#include"STRUCT_CALCDIFF.h"
#include"STRUCT_OP.h"
#include"STRUCT_SEQ.h"
#include"STRUCT_NOTE.h"

#include"GAME_LOAD.h"
#include"LOAD.h"
#include"folder_insert.h"
#include"secret_LOAD.h"
#include"STRUCT_SECRET.h"
#include"LoadSkillTestNpsPathList.h"
#include"OptionStateSaveLoad.h"
#include"ShowErrorName.h"
#include"SongCacheSaveLoad.h"

void LOAD(LIST *song_folder, Song *Music, int *NumberOfSongs, SECRET *secret, SkillTestList *STList, OPTION *op, CONFIG config) {
	int i = 0, j = 0;

	HANDLE hFind_Folders = 0;//songsフォルダからフォルダを見つける用ハンドル
	HANDLE hFind_Songs = 0;//Songsフォルダ/~~~ から曲フォルダを見つける用ハンドル
	HANDLE hFind_nps = 0;//曲フォルダからnpsファイルを見つける用
	
	WIN32_FIND_DATA folder_lp;
	WIN32_FIND_DATA song_lp;
	WIN32_FIND_DATA file_lp;

	wchar_t folder[256] = L"\0";//フォルダーの名前を入れておく用の変数
	wchar_t Path[256] = L"\0";
	wchar_t strbuf[512] = L"\0";
	//NOTE dummy[5][NOTE_MAX_NUMBER];
	int lane_dummy[5];
	CALCDIFF Cdiff_dummy;
	OPTION opt_dummy;
	BPMC bpmchange[NOTE_MAX_NUMBER];
	SC scrollchange[NOTE_MAX_NUMBER];
	STOP_SE stopSequence[NOTE_MAX_NUMBER];
	int hash = 0;
	int TimeToEndScrollDummy=0;
	int playing_time_dummy = 0;

	NOTE **note;
	note = (NOTE**)calloc(4, sizeof(NOTE));

	for (i = 0; i <= 3; i++) {
		note[i] = (NOTE*)calloc(NOTE_MAX_NUMBER, sizeof(NOTE));
	}

	BARLINE *barline;
	barline = (BARLINE*)calloc(NOTE_MAX_NUMBER, sizeof(BARLINE));

	SongCache Cache;

	//曲構造体のNULLを\0でデータ格納
	for (i = 0; i < SONG_MAX_NUMBER; i++) {
		for (j = 0; j < 5; j++) {
			Music[i].title[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].title[j], L"\0");
			Music[i].genre[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].genre[j], L"\0");
			Music[i].artist[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].artist[j], L"\0");
			Music[i].wavpath[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].wavpath[j], L"\0");
			Music[i].jacketpath[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].jacketpath[j], L"\0");
			Music[i].jacketpath2[j] = (wchar_t*)calloc(strlenDx(L"\0") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].jacketpath2[j], L"\0");
			Music[i].jinglepath[j] = (wchar_t*)calloc(strlenDx(L"sound/jingle.wav") + 1, sizeof(wchar_t));
			sprintfDx(Music[i].jinglepath[j], L"sound/jingle.wav");
		}
	}



	if (ProcessMessage() != 0) {
		DxLib_End();
		return;
	}
	i = 0;


	//----フォルダ探し----
	hFind_Folders = FindFirstFile(L"songs/*", &folder_lp);//Songsフォルダの最初の探索
	if (hFind_Folders == INVALID_HANDLE_VALUE) {
		ShowError(L"songsフォルダがありません。");
		DxLib_End();
		return;
	}
	do {
		if ((wcscmp(folder_lp.cFileName, L".") != 0) && (wcscmp(folder_lp.cFileName, L"..") != 0) && (folder_lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた

		    //----曲フォルダ探し----(Music.SongPath[][]にnpsファイルのパスを入れる)
			sprintfDx(Path, L"songs/%s/*", folder_lp.cFileName);
			hFind_Songs = FindFirstFile(Path, &song_lp);//Song/~~~フォルダの最初の探索
			if (hFind_Songs == INVALID_HANDLE_VALUE) {
				ShowError(L"songsフォルダの各フォルダ中の探索に失敗しました。");
				DxLib_End();
				return;
			}
			do {
				if (ProcessMessage() != 0) {
					DxLib_End();
					return;
				}

				/*
				if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (wcsstr(lp.cFileName, L".dxa") != 0)) {//自身と親以外の.dxaファイルを見つけた
					if (0 == wcscmp(wcsstr(lp.cFileName, L".dxa"), L".dxa")) {//.dxaのファイルを見つけた
																			//printfDx("曲フォルダ発見!\n");
						wcsncpy_s(folder, lp.cFileName, wcslen(lp.cFileName) - 4);//フォルダ(.dxa除いた部分)名を格納

						wcscpy_s(Path, L"songs/official/");
						wcscat_s(Path, folder);
						wcscpy_s(Music[i].FolderPath, Path);//フォルダのパスを保存

						wcscpy_s(Music[i].SaveFolder, L"score/official/");//セーブデータフォルダー名を格納
						wcscat_s(Music[i].SaveFolder, folder);

						wcscpy_s(Music[i].RivalSaveFolder, L"score_rival/official/");//ライバルセーブデータフォルダー名を格納
						wcscat_s(Music[i].RivalSaveFolder, folder);

						wcscpy_s(Music[i].SongPath[1], Path);//npsのパスを保存
						wcscat_s(Music[i].SongPath[1], L"/1.nps");
						wcscpy_s(Music[i].SongPath[2], Path);//npsのパスを保存
						wcscat_s(Music[i].SongPath[2], L"/2.nps");
						wcscpy_s(Music[i].SongPath[3], Path);//npsのパスを保存
						wcscat_s(Music[i].SongPath[3], L"/3.nps");
						wcscpy_s(Music[i].SongPath[4], Path);//npsのパスを保存
						wcscat_s(Music[i].SongPath[4], L"/4.nps");
						GAME_LOAD(i, 1, note, lane_dummy, 0, &Cdiff_dummy, &opt_dummy, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy,&playing_time_dummy);
						Music[i].hash[1] = hash;
						folder_insert(song_folder, i, 1, Music);

						GAME_LOAD(i, 2, note, lane_dummy, 0, &Cdiff_dummy, &opt_dummy, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy,&playing_time_dummy);
						Music[i].hash[2] = hash;
						folder_insert(song_folder, i, 2, Music);

						GAME_LOAD(i, 3, note, lane_dummy, 0, &Cdiff_dummy, &opt_dummy, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy,&playing_time_dummy);
						Music[i].hash[3] = hash;
						folder_insert(song_folder, i, 3, Music);

						GAME_LOAD(i, 4, note, lane_dummy, 0, &Cdiff_dummy, &opt_dummy, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy,&playing_time_dummy);
						Music[i].hash[4] = hash;
						folder_insert(song_folder, i, 4, Music);


						folder_insert_season(song_folder, i, Music);
						//song_folder->folder[0][song_folder->folder_c[0]].song_number = i;
						//song_folder->folder_c[0]++;
						i++;//曲番号
					}
				}
				*/

				if ((wcscmp(song_lp.cFileName, L".") != 0) && (wcscmp(song_lp.cFileName, L"..") != 0) && (song_lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
					//printfDx("曲フォルダ発見!\n");
					//wcscpy_s(folder, lp.cFileName);
					//wcscpy_s(Path, L"songs/official/");
					//wcscat_s(Path, folder);
					//wcscpy_s(Music[i].FolderPath, Path);//フォルダのパスを保存

					sprintfDx(strbuf, L"%s", folder_lp.cFileName);
					Music[i].KindFolder = (wchar_t*)malloc(sizeof(wchar_t) * (strlenDx(strbuf) + 1));
					sprintfDx(Music[i].KindFolder, strbuf);

					sprintfDx(strbuf, L"songs/%s/%s", folder_lp.cFileName, song_lp.cFileName);
					Music[i].FolderPath = (wchar_t*)malloc(sizeof(wchar_t)*(strlenDx(strbuf)+1));
					sprintfDx(Music[i].FolderPath, strbuf);

					//wcscat_s(Path, L"/*.nps");//SongPath[i]はsongs/folder/*.npsになる

					//wcscpy_s(Music[i].SaveFolder, L"score/official/");//セーブデータフォルダー名を格納
					//wcscat_s(Music[i].SaveFolder, folder);

					sprintfDx(strbuf, L"score/%s/%s", folder_lp.cFileName, song_lp.cFileName);
					Music[i].SaveFolder = (wchar_t*)malloc(sizeof(wchar_t) * (strlenDx(strbuf) + 1));
					sprintfDx(Music[i].SaveFolder, strbuf);

					sprintfDx(strbuf, L"cache/%s/%s", folder_lp.cFileName, song_lp.cFileName);
					Music[i].CacheFolder = (wchar_t*)malloc(sizeof(wchar_t) * (strlenDx(strbuf) + 1));
					sprintfDx(Music[i].CacheFolder, strbuf);

					//wcscpy_s(Music[i].RivalSaveFolder, L"score_rival/official/");//ライバルセーブデータフォルダー名を格納
					//wcscat_s(Music[i].RivalSaveFolder, folder);

					sprintfDx(strbuf, L"score_rival/%s/%s", folder_lp.cFileName, song_lp.cFileName);
					Music[i].RivalSaveFolder = (wchar_t*)malloc(sizeof(wchar_t) * (strlenDx(strbuf) + 1));
					sprintfDx(Music[i].RivalSaveFolder, strbuf);

					sprintfDx(Path, L"songs/%s/%s/*.nps", folder_lp.cFileName, song_lp.cFileName);

					//----npsファイル探し----
					hFind_nps = FindFirstFile(Path, &file_lp);
					if (hFind_nps == INVALID_HANDLE_VALUE) {
						ShowError(L"npsファイルが一つもない曲フォルダがあります。");
						DxLib_End();
						return;
					}
					do {
						//wcscpy_s(Path, L"songs/official/");
						//wcscat_s(Path, folder);
						//wcscat_s(Path, L"/");
						//wcscat_s(Path, lp.cFileName);


						j = 0;
						if (wcscmp(file_lp.cFileName, L"1.nps") == 0) {
							j = 1;
							Music[i].exist[j] = 1;
						}
						if (wcscmp(file_lp.cFileName, L"2.nps") == 0) {
							j = 2;
							Music[i].exist[j] = 1;
						}
						if (wcscmp(file_lp.cFileName, L"3.nps") == 0) {
							j = 3;
							Music[i].exist[j] = 1;
						}
						if (wcscmp(file_lp.cFileName, L"4.nps") == 0) {
							j = 4;
							Music[i].exist[j] = 1;
						}
						file_lp.ftLastWriteTime;
						sprintfDx(strbuf, L"songs/%s/%s/%s", folder_lp.cFileName, song_lp.cFileName, file_lp.cFileName);
						Music[i].SongPath[j] = (wchar_t*)malloc(sizeof(wchar_t) * (strlenDx(strbuf) + 1));
						sprintfDx(Music[i].SongPath[j], strbuf);

						//wcscpy_s(Music[i].SongPath[j], Path);
					

						int readHeaderOnly = 0;
						if (LoadSongCache(&Cache, Music, i, j) == 0) {//キャッシュが見つかって読み込んだ時
							//SYSTEMTIME writeTimeFile, writeTimeCache;
							//FileTimeToSystemTime(&file_lp.ftLastWriteTime, &writeTimeFile);
							//FileTimeToSystemTime(&Cache.writeTime, &writeTimeCache);

							if ((file_lp.ftLastWriteTime.dwHighDateTime == Cache.writeTime.dwHighDateTime) &&
								(file_lp.ftLastWriteTime.dwLowDateTime == Cache.writeTime.dwLowDateTime)) {//キャッシュと更新時間が同じなのでヘッダだけの読み込みをする
								readHeaderOnly = 1;
								writeCacheToMusic(&Cache, Music, i, j);
							}
						}
						GAME_LOAD(i, j, note, barline, lane_dummy, readHeaderOnly, &Cdiff_dummy, &opt_dummy, bpmchange, scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy, &playing_time_dummy, config);
						if (readHeaderOnly == 0) {
							Cache.writeTime = file_lp.ftLastWriteTime;
							writeMusicToCache(&Cache, Music, i, j);
							SaveSongCache(Cache, Music, i, j);
						}

						folder_insert(song_folder, i, j, Music);

						Music[i].hash[j] = hash;
					} while (FindNextFile(hFind_nps, &file_lp));

					//隠し曲設定があるか確認
					if (secret_LOAD(Music, i) == 1) {//隠し曲だったら
						secret->song_number[secret->total] = i;//隠し曲番号を格納

						//解禁しているか確認
						secret_save_LOAD(secret, secret->total, Music[i].SaveFolder);//隠し曲のセーブファイル確認
						if (secret->get_song_number[secret->total] == 1) {//解禁していた
							Music[i].secret = 2;
						}
						secret->total++;//隠し曲総数+1
					}

					folder_insert_season(song_folder, i, Music);
					//song_folder->folder[0][song_folder->folder_c[0]].song_number = i;
					//song_folder->folder_c[0]++;
					i++;//曲番号

					//読み込み経過表示
					ClearDrawScreen();
					clsDx();
					printfDx(L"loading(%d曲目)\n", i);
					printfDx(L"無線LANに接続するとAndroidコントローラを使用できます\n");
					ScreenFlip();

					if (FindClose(hFind_nps) == 0) {
						DxLib_End();
						return;
					}
				}
			} while (FindNextFile(hFind_Songs, &song_lp));//何も見つからなかったら0になりループを抜ける

			if (FindClose(hFind_Songs) == 0) {
				DxLib_End();
				return;
			}
		}

	}while(FindNextFile(hFind_Folders, &folder_lp));//何も見つからなかったら0になりループを抜ける

	if (FindClose(hFind_Folders) == 0) {
		DxLib_End();
		return;
	}

	/*
	//userフォルダの探索
	wcscpy_s(Path, L"songs/user/");
	hFind_Songs = FindFirstFile(L"songs/user/*", &lp);//Songフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}

		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
																																		 //printfDx("曲フォルダ発見!\n");
			wcscpy_s(folder, lp.cFileName);
			wcscpy_s(Path, L"songs/user/");
			wcscat_s(Path, folder);
			wcscpy_s(Music[i].FolderPath, Path);//フォルダのパスを保存
			wcscat_s(Path, L"/*.nps");//SongPath[i]はsongs/folder/*.npsになる

			wcscpy_s(Music[i].SaveFolder, L"score/user/");//セーブデータフォルダー名を格納
			wcscat_s(Music[i].SaveFolder, folder);

			wcscpy_s(Music[i].RivalSaveFolder, L"score_rival/user/");//ライバルセーブデータフォルダー名を格納
			wcscat_s(Music[i].RivalSaveFolder, folder);

			hFind_nps = FindFirstFile(Path, &lp);
			do {
				wcscpy_s(Path, L"songs/user/");
				wcscat_s(Path, folder);
				wcscat_s(Path, L"/");
				wcscat_s(Path, lp.cFileName);
				j = 0;
				if (wcscmp(lp.cFileName, L"1.nps") == 0) {
					j = 1;
					Music[i].exist[j] = 1;
				}
				if (wcscmp(lp.cFileName, L"2.nps") == 0) {
					j = 2;
					Music[i].exist[j] = 1;
				}
				if (wcscmp(lp.cFileName, L"3.nps") == 0) {
					j = 3;
					Music[i].exist[j] = 1;
				}
				if (wcscmp(lp.cFileName, L"4.nps") == 0) {
					j = 4;
					Music[i].exist[j] = 1;
				}

				wcscpy_s(Music[i].SongPath[j], Path);
				GAME_LOAD(i, j, note, lane_dummy, 0, &Cdiff_dummy, &opt_dummy, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScrollDummy,&playing_time_dummy);
				folder_insert(song_folder, i, j, Music);

				Music[i].hash[j] = hash;
			} while (FindNextFile(hFind_nps, &lp));

			//隠し曲設定があるか確認
			if (secret_LOAD(Music, i) == 1) {//隠し曲だったら
				secret->song_number[secret->total] = i;//隠し曲番号を格納

													   //解禁しているか確認
				secret_save_LOAD(secret, secret->total, Music[i].SaveFolder);//隠し曲のセーブファイル確認
				if (secret->get_song_number[secret->total] == 1) {//解禁していた
					Music[i].secret = 2;
				}
				secret->total++;//隠し曲総数+1
			}

			folder_insert_season(song_folder, i, Music);
			//song_folder->folder[0][song_folder->folder_c[0]].song_number = i;
			//song_folder->folder_c[0]++;
			i++;//曲番号
			if (FindClose(hFind_nps) == 0) {
				DxLib_End();
				return;
			}
		}
	} while (FindNextFile(hFind_Songs, &lp));//何も見つからなかったら0になりループを抜ける
	*/

	//隠し曲が全解禁されているか確認
	secret_all_get(secret);//全解禁されているならall_getは1

	*NumberOfSongs = i;
	//Musicの最後に「フォルダ選択へ戻る」要素を追加
	wchar_t StrBackToFolderSelect[] = L"フォルダ選択へ戻る";
	for (i = 0; i <= 4; i++) {
		free(Music[*NumberOfSongs].title[i]);
		Music[*NumberOfSongs].title[i] = (wchar_t*) calloc(strlenDx(StrBackToFolderSelect) + 1, sizeof(wchar_t));
		sprintfDx(Music[*NumberOfSongs].title[i], L"%s", StrBackToFolderSelect);
		Music[*NumberOfSongs].exist[i] = 0;
		Music[*NumberOfSongs].StrColor[i] = GetColor(180, 180, 180);
		Music[*NumberOfSongs].StrShadowColor[i] = GetColor(0, 0, 0);

	}
	
	//曲読み込み終了

	//各フォルダの最後に「フォルダ選択へ戻る」要素を追加
	for (i = 0; i <= song_folder->NumberOfFolders - 1; i++) {
		song_folder->folder[i][song_folder->folder_c[i]].difficulty = 0;//フォルダ選択へ戻る
		song_folder->folder[i][song_folder->folder_c[i]].kind = 1;//フォルダ選択へ戻る
		song_folder->folder[i][song_folder->folder_c[i]].song_number = *NumberOfSongs;
		song_folder->folder_c[i]++;//枠をひとつ追加
	}


	int SongNumberList[4];
	int DifficultyList[4];

	short bpmmin, bpmmax;

	//段位のbpm min maxを格納
	for (i = 0; i <= NUMBER_OF_COURSES - 1; i++) {
		LoadSkillTestNpsPathList(SongNumberList, DifficultyList, STList->fliepath[i], Music, *NumberOfSongs);
		

		bpmmin = Music[SongNumberList[0]].bpmmin[DifficultyList[0]];
		bpmmax = Music[SongNumberList[0]].bpmmax[DifficultyList[0]];
		//min max bpmを計算
		for (j = 1; j <= 3; j++) {
			if (bpmmin >= Music[SongNumberList[j]].bpmmin[DifficultyList[j]]) {
				bpmmin = Music[SongNumberList[j]].bpmmin[DifficultyList[j]];
			}
			if (bpmmax <= Music[SongNumberList[j]].bpmmax[DifficultyList[j]]) {
				bpmmax = Music[SongNumberList[j]].bpmmax[DifficultyList[j]];
			}
		}
		STList->bpmmin[i] = bpmmin;
		STList->bpmmax[i] = bpmmax;
	}

	SearchNoteSkin(op);
	SearchHitSoundSkin(op);


	//最後のプレーのオプション読み込み
	LoadOptionState(op);


	printfDx(L"曲数:%d\n", NumberOfSongs);
	for (i = 0; i <= *NumberOfSongs - 1; i++) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}
		//printfDx("%s\n", Music[i].SongPath[1]);
		//printfDx("%s\n", Music[i].SongPath[2]);
		//printfDx("%s\n", Music[i].SongPath[3]);
		clsDx();
		ScreenFlip();//画面の反映
	}

	clsDx();
	for (i = 0; i <= 3; i++) {
		free(note[i]);
	}
	free(note);
	return;
}

void SearchNoteSkin(OPTION *op) {//ノートスキン読み込み
	HANDLE hFind_Notes = 0;//見つける用ハンドル
	WIN32_FIND_DATA lp;
	

	//まず個数を調べる
	op->NOTE_NUM = 0;
	hFind_Notes = FindFirstFile(L"img/notes/*", &lp);//notesフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
			op->NOTE_NUM++;
		}
	} while (FindNextFile(hFind_Notes, &lp));//何も見つからなかったら0になりループを抜ける
	if (FindClose(hFind_Notes) == 0) {
		ShowError(L"音符画像フォルダがありません。");
		DxLib_End();
		return;
	}

	//メモリ確保(ノートスキンの種類分)
	op->note = new wchar_t*[op->NOTE_NUM];

	op->ArrayOptionKindName[op->OP_NOTE] = op->note;//選曲画面で使うオプションの種類→名称の配列にアドレスを格納


	//実際の名前を入れていく
	op->NOTE_NUM = 0;
	hFind_Notes = FindFirstFile(L"img/notes/*", &lp);//notesフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
			//文字列分のメモリ確保
			int len = wcslen(lp.cFileName);
			op->note[op->NOTE_NUM] = new wchar_t[wcslen(lp.cFileName)+1];//終端文字分を考えて文字数+1のメモリを取る
			//文字列格納
			sprintfDx(op->note[op->NOTE_NUM], L"%s", lp.cFileName);
			op->NOTE_NUM++;
		}
	} while (FindNextFile(hFind_Notes, &lp));//何も見つからなかったら0になりループを抜ける




	if (FindClose(hFind_Notes) == 0) {
		DxLib_End();
		return;
	}
}



void SearchHitSoundSkin(OPTION *op) {//ヒット音スキン読み込み
	HANDLE hFind_HitSounds = 0;//見つける用ハンドル
	WIN32_FIND_DATA lp;


	//まず個数を調べる
	op->HITSOUND_NUM = 0;
	hFind_HitSounds = FindFirstFile(L"sound/hit_sound/*", &lp);//hit_soundフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
			op->HITSOUND_NUM++;
		}
	} while (FindNextFile(hFind_HitSounds, &lp));//何も見つからなかったら0になりループを抜ける
	if (FindClose(hFind_HitSounds) == 0) {
		ShowError(L"ヒット音フォルダがありません。");
		DxLib_End();
		return;
	}

	//メモリ確保(ノートスキンの種類分)
	op->hitsound = new wchar_t*[op->HITSOUND_NUM];

	op->ArrayOptionKindName[op->OP_HITSOUND] = op->hitsound;

	//実際の名前を入れていく
	op->HITSOUND_NUM = 0;
	hFind_HitSounds = FindFirstFile(L"sound/hit_sound/*", &lp);//notesフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			DxLib_End();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた
																																		   //文字列分のメモリ確保
			int len = wcslen(lp.cFileName);
			op->hitsound[op->HITSOUND_NUM] = new wchar_t[wcslen(lp.cFileName) + 1];//終端文字分を考えて文字数+1のメモリを取る
																		   //文字列格納
			sprintfDx(op->hitsound[op->HITSOUND_NUM], L"%s", lp.cFileName);
			op->HITSOUND_NUM++;
		}
	} while (FindNextFile(hFind_HitSounds, &lp));//何も見つからなかったら0になりループを抜ける

	if (FindClose(hFind_HitSounds) == 0) {
		DxLib_End();
		return;
	}
}

