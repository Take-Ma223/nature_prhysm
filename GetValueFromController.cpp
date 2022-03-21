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

//�l�̍X�V���������Ƃ�1��Ԃ�
int GetValueFromController::getVal(int *result) {
	int val = f1.get();

	if (val != -1) {//��M����
		if (*result == easing(val)) {//�l�̍X�V����
			return 0;
		}
		else {
			*result = easing(val);//�l�̍X�V
			return 1;
		}
	}

	return 0;//��M���s(�^�C���A�E�g)
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