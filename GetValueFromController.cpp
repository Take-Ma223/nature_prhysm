#include"GetValueFromController.h"
#include <future>
#include <thread>

GetValueFromController::GetValueFromController(HANDLE rcvHComm) {
	hComm = rcvHComm;
}

int GetValueFromController::getValFromController(int value) {
	unsigned char receiveData[1024];
	unsigned long lRead;

	DWORD dwErrors;
	COMSTAT ComStat;
	DWORD dwCount;


	ClearCommError(hComm, &dwErrors, &ComStat);
	dwCount = ComStat.cbInQue;
	BOOL Ret = ReadFile(// データの受信
		hComm,          // 　通信デバイスのハンドル：　CreateFile()で取得したハンドルを指定
		receiveData,   // 受信バッファーのポインタを指定：　受信データがここに格納されます。
		dwCount,        //　受信するバイト数を指定：　ここで指定するバイト数を受信するかまたはタイムアウト時間がくるまで
						// ReadFile()関数は（　getc()のように　）待ちます
		&lRead,         //  実際に受信したバイト数（DWORD)が格納されるポインタを指定
		NULL   // 通信とは関係ない引数なのでNULLを指定
	);
	int ge = GetLastError();

	if (Ret && lRead > 0 && lRead == dwCount) {
		value = (int)receiveData[dwCount - 1];
		return value;
	}

	return -1;
}

void GetValueFromController::start() {
	f1 =
	std::async(std::launch::async, [this] {return this->getValFromController(val);});
	
}

void GetValueFromController::finish() {
	finishFlag = TRUE;
}

//値の更新があったとき1を返す
int GetValueFromController::getVal(int *result) {
	int val = f1.get();

	if (val != -1) {//受信成功
		if (*result == easing(val)) {//値の更新無し
			return 0;
		}
		else {
			*result = easing(val);//値の更新
			return 1;
		}
	}

	return 0;//受信失敗(タイムアウト)
}


int GetValueFromController::easing(int input)
{
	if (input < 3) {
		return 0;
	}
	else if (input >= 253) {
		return 1000;
	}
	else {
		int output = 4 * (input - 3);
		return output;
	}

}