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
	BOOL Ret = ReadFile(// �f�[�^�̎�M
		hComm,          // �@�ʐM�f�o�C�X�̃n���h���F�@CreateFile()�Ŏ擾�����n���h�����w��
		receiveData,   // ��M�o�b�t�@�[�̃|�C���^���w��F�@��M�f�[�^�������Ɋi�[����܂��B
		dwCount,        //�@��M����o�C�g�����w��F�@�����Ŏw�肷��o�C�g������M���邩�܂��̓^�C���A�E�g���Ԃ�����܂�
						// ReadFile()�֐��́i�@getc()�̂悤�Ɂ@�j�҂��܂�
		&lRead,         //  ���ۂɎ�M�����o�C�g���iDWORD)���i�[�����|�C���^���w��
		NULL   // �ʐM�Ƃ͊֌W�Ȃ������Ȃ̂�NULL���w��
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

int GetValueFromController::getVal() {
	int result = f1.get();

	if (result != -1) {
		val = result;
	}

	return val;
}


