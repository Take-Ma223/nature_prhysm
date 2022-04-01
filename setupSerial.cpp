#include"DxLib.h"

void setupSerial(HANDLE *hComm) {//シリアル通信

	int fp;
	wchar_t buf1[10];
	wchar_t buf2[10];
	int number = 0;//ポート番号
	wchar_t COM[10] = L"COM3";//シリアルポートの文字列

							  /*
							  error = fopen_s(&fp, "COMport.dat", "r");
							  if (error != 0) {
							  exit(1);
							  return;
							  }
							  */

	fp = FileRead_open(L"save_data/COMport.dat");
	if (fp == 0) {//無かったら戻る
		return;
	}
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし

	wchar_t str[10];


	FileRead_gets(str, 10, fp);//一行取得(\nは入らない)
	swscanf_s(str, L"%[^:\n]:%[^:\n]\n", buf1, _countof(buf1), buf2, _countof(buf2));
	/*
	printfDx("%s\n",ButtonName);
	ScreenFlip();
	Sleep(10);
	*/

	int buf = wcscmp(buf1, L"COM");

	if (buf == 0){
		number = _wtoi(buf2);
		swprintf_s(COM, L"COM%d", number);
	}

	FileRead_close(fp);

	//1.COMポートハンドルの取得 ポートをファイルとみなして　CreateFile（）関数でオープンしてハンドルを取得します。
	*hComm = CreateFile(
		COM,                       /* シリアルポートの文字列 */
		GENERIC_READ | GENERIC_WRITE, /* アクセスモード */
		0,/* 共有モード */
		NULL,                         /* セキュリティ属性 */
		OPEN_EXISTING,                /* 作成フラグ */
		FILE_ATTRIBUTE_NORMAL,        /* 属性 */
		NULL                          /* テンプレートのハンドル */
	);

	if (*hComm == INVALID_HANDLE_VALUE)
	{
		/* エラー処理 */
	}

	//2.送受信バッファ初期化
    SetupComm(  //設定
		*hComm,  // 　通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		1024,   //   受信バッファーサイズ：　受信のバッファーサイズをバイト単位で指定
		1024    // 　送信バッファーサイズ：　送信のバッファーサイズをバイト単位で指定
	);

	int Ret = PurgeComm(//消去
		*hComm,  //通信デバイスのハンドル：CreateFile()で取得したハンドルを指定
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR
		//   実行する操作： 上記は未処理の読書きの中止及び送受信のバッファーのクリアを指定
	);



	//3.基本通信条件の設定
	DCB dcb;
	GetCommState(*hComm, &dcb); /* DCB を取得 */
	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.fParity = FALSE;
	dcb.StopBits = ONESTOPBIT;
	//dcb.fTXContinueOnXoff = FALSE; // 受信バッファー満杯＆XOFF受信後の継続送信可否：送信可→TRUE
	SetCommState(*hComm, &dcb); /* DCB を設定 */


	//4.タイムアウト時間の設定
	COMMTIMEOUTS timeout; // COMMTIMEOUTS構造体の変数を宣言

	timeout.ReadIntervalTimeout = 0; // 文字読込時の２も時間の全体待ち時間（msec）

	timeout.ReadTotalTimeoutMultiplier = 0; //読込の１文字あたりの時間
	timeout.ReadTotalTimeoutConstant = 1; //読込エラー検出用のタイムアウト時間
		 //(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier * (受信予定バイト数) + ReadTotalTimeoutConstant

	timeout.WriteTotalTimeoutMultiplier = 0; //書き込み１文字あたりの待ち時間
	timeout.WriteTotalTimeoutConstant = 1;//書き込みエラー検出用のタイムアウト時間
		 //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier * (送信予定バイト数) + WriteTotalTimeoutConstant

	Ret = SetCommTimeouts(*hComm, &timeout);//SetCommTimeOut()関数にポートハンドルおよびCOMMTIMEOUTS構造体の
														  //アドレスを代入します。




}