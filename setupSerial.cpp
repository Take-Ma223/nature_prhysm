#include"DxLib.h"
#include "STRUCT_CONFIG.h"

void setupSerial(HANDLE *hComm, Config &config) {//シリアル通信

	wchar_t COM[10] = L"COM3";//シリアルポートの文字列
	swprintf_s(COM, L"COM%d", config.ComPort);

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
	dcb.BaudRate = 300;
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
	timeout.WriteTotalTimeoutConstant = 1000;//書き込みエラー検出用のタイムアウト時間
		 //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier * (送信予定バイト数) + WriteTotalTimeoutConstant

	Ret = SetCommTimeouts(*hComm, &timeout);//SetCommTimeOut()関数にポートハンドルおよびCOMMTIMEOUTS構造体の
														  //アドレスを代入します。




}