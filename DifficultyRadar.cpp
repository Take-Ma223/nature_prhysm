#include "DifficultyRadar.h"
#include<math.h>
#include<numeric>


DifficultyRadar::DifficultyRadar(
	NOTE** note, 
	int* nc, 
	BPMC* bpmchange, 
	STOP_SE* stopSequence, 
	SC* scrollchange, 
	int time, 
	int StartTime, 
	int EndTime, 
	int* TimingSame, 
	double BPM_suggest, 
	std::shared_ptr<std::vector<float>> bpmList) {
	DifficultyRadar::note = note;
	DifficultyRadar::nc = nc;
	DifficultyRadar::bpmchange = bpmchange;
	DifficultyRadar::stopSequence = stopSequence;
	DifficultyRadar::scrollchange = scrollchange;
	DifficultyRadar::time = time;
	DifficultyRadar::StartTime = StartTime;
	DifficultyRadar::EndTime = EndTime;
	DifficultyRadar::TimingSame = TimingSame;
	DifficultyRadar::BPM_suggest = BPM_suggest;
	DifficultyRadar::bpmList = bpmList;

}

void DifficultyRadar::GetLocalNotesGraph(short* LocalNotesGraph, bool isRainbow) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//�Ǐ��F��Փx
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing same�̃J�E���^�[
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//���̋�Ԃ̍ŏ��̉����̃^�C�~���O
	int et = 0;//���̋�Ԃ̍Ō�̉����̃^�C�~���O


	//BoxTime�b�̘g�����炵�Ȃ���g�ɓ��鉹���̖��x�̍ō��l��Local�l�Ƃ���
	int BoxTime = int((double)time / 9 + 0.5);//�g�̎���(ms)
	int SlideTime = BoxTime;//�g���X���C�h���鎞��
	int Count[4] = { 0,0,0,0 };//�g�̒��ɓ����Ă��鉹���̐�
	int SlideCount = 0;//�X���C�h�񐔃J�E���g

	int CountToSlide = int((double)time / SlideTime + 1);//�X���C�h�����

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };

	if (isRainbow) {
		NoteCountVal[0] = 1;
		NoteCountVal[1] = 1;
		NoteCountVal[2] = 1;
		NoteCountVal[3] = 1;
	}

	do {
		//4���[�����ꂼ��̘g�����������v�Z
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//�V�����g���ɓ����Ă����������𑫂�
				if (nc_last[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_last[ind]].group == NoteGroup::Single || note[ind][nc_last[ind]].group == NoteGroup::LongNoteStart)
					&& note[ind][nc_last[ind]].color != NoteColor::K) {//�g�I��肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_last[ind]].color == NoteColor::R ||
						note[ind][nc_last[ind]].color == NoteColor::G ||
						note[ind][nc_last[ind]].color == NoteColor::B) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::Y ||
						note[ind][nc_last[ind]].color == NoteColor::C ||
						note[ind][nc_last[ind]].color == NoteColor::M) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::W) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::F) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//�g������o�Ă�����������������
				if (nc_start[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_start[ind]].group == NoteGroup::Single || note[ind][nc_start[ind]].group == NoteGroup::LongNoteStart)//�g������o�Ă�����������������
					&& note[ind][nc_start[ind]].color != NoteColor::K) {//�g�n�܂肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_start[ind]].color == NoteColor::R ||
						note[ind][nc_start[ind]].color == NoteColor::G ||
						note[ind][nc_start[ind]].color == NoteColor::B) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::Y ||
						note[ind][nc_start[ind]].color == NoteColor::C ||
						note[ind][nc_start[ind]].color == NoteColor::M) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::W) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::F) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		if (SlideCount <= 8) {
			LocalNotesGraph[SlideCount] = int(25000 * (double)NoteCount / BoxTime);//�������x�O���t���쐬
		}

		SlideCount++;
	} while (SlideCount < CountToSlide);

	return;
}

void DifficultyRadar::GetColorNotesGraph(short* colorNotesGraph, short* colorNotesGraphR)
{

	int i = 0;
	for (i = 0; i < 9; i++) {//�e�F�������̏�����
		colorNotesGraph[i] = 0;
		colorNotesGraphR[i] = 0;
	}

	int lane = 0, NoteCounter = 0;
	//�����̍��v���擾
	for (lane = 0; lane <= 3; lane++) {
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].group == NoteGroup::Single || note[lane][NoteCounter].group == NoteGroup::LongNoteStart) {
				if (note[lane][NoteCounter].color != NoteColor::K) {

					if (note[lane][NoteCounter].color == NoteColor::R ||
						note[lane][NoteCounter].color == NoteColor::G ||
						note[lane][NoteCounter].color == NoteColor::B) {//RGB
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::Y ||
						note[lane][NoteCounter].color == NoteColor::C ||
						note[lane][NoteCounter].color == NoteColor::M) {//CMY
						TotalNotes += 2;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::W) {//W
						TotalNotes += 3;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::F) {//F
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
				}
				TotalNotesK++;
			}

			//�e�F���������邩���Z
			if (note[lane][NoteCounter].group == NoteGroup::LongNoteMiddle || note[lane][NoteCounter].group == NoteGroup::LongNoteEnd) {
				if (note[lane][NoteCounter].color != note[lane][NoteCounter - 1].color) {
					//LN���ԁALN�I�_�͐F���ς���Ă���ꍇ�̂݉��Z
					colorNotesGraph[NumberTranslation(note[lane][NoteCounter].color)]++;
					colorNotesGraphR[NumberTranslation(note[lane][NoteCounter].color)]++;
				}
			}
			else {
				colorNotesGraph[NumberTranslation(note[lane][NoteCounter].color)]++;
				colorNotesGraphR[NumberTranslation(note[lane][NoteCounter].color)]++;

				////�����[�h�p
				//if (note[lane][NoteCounter].color == NoteColor::K) {
				//	colorNotesGraphR[NumberTranslation(NoteColor::K)]++;
				//}
				//else {
				//	colorNotesGraphR[NumberTranslation(NoteColor::F)]++;
				//}

			}

			if (note[lane][NoteCounter].group == NoteGroup::LongNoteEnd && note[lane][NoteCounter].LN_k == 1) {//LN�I�[�̍�
				colorNotesGraph[NumberTranslation(NoteColor::K)]++;
				colorNotesGraphR[NumberTranslation(NoteColor::K)]++;
			}
		}
	}

	for (i = 0; i < 9; i++) {//�e�F�������𕽋ϖ��x�ɂ��đΐ������
		colorNotesGraph[i] = int(200 * log10((double)colorNotesGraph[i] * 30000 / (DifficultyRadar::time)+1));
		colorNotesGraphR[i] = int(200 * log10((double)colorNotesGraphR[i] * 30000 / (DifficultyRadar::time)+1));
	}

}

int DifficultyRadar::NumberTranslation(NoteColor color) {
	int val = (int)color;

	val--;//0~8�ɂ���

	return val;
}


int DifficultyRadar::CalcGlobal(bool isRainbow) {
	double GlobalScore = 0;

	if (!isRainbow) {
		GlobalScore = TotalNotes;
	}
	else{
		GlobalScore = TotalNotesRainbow;
	}

	double LN_point = 545.454545454545;//LN��1point������̕b��(ms)
	double color_point[11] = { 0,1,1,1,2,2,2,3,0,0.9,0 };//�����̐F���̓�Փx�|�C���g
	double cp_CAP = 1.333;//���鉹���̓�Փx�|�C���g�{��
	int time_use = time;
	int time_limit = 12000;//����ȉ��̎��ԂŖ��x���o���Ȃ�


	if (time_use <= time_limit)time_use = time_limit;

	//1��������̑��m�[�g�����Z�o
	GlobalScore = (GlobalScore / ((double)time_use / 1000)) * 60;

	return (int)(GlobalScore * 100 / globalMax);
}

int DifficultyRadar::CalcLocal(bool isRainbow) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//�Ǐ��F��Փx
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing same�̃J�E���^�[
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//���̋�Ԃ̍ŏ��̉����̃^�C�~���O
	int et = 0;//���̋�Ԃ̍Ō�̉����̃^�C�~���O


	//BoxTime�b�̘g�����炵�Ȃ���g�ɓ��鉹���̖��x�̍ō��l��Local�l�Ƃ���
	int BoxTime = 6000;//�g�̎���(ms)
	int SlideTime = 100;//�g���X���C�h���鎞��
	int Count[4] = { 0,0,0,0 };//�g�̒��ɓ����Ă��鉹���̐�
	int SlideCount = 0;//�X���C�h�񐔃J�E���g

	int CountToSlide = int((double)time / SlideTime + 1);//�X���C�h�����

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };
	if (isRainbow) {
		NoteCountVal[RGB] = 1;
		NoteCountVal[CMY] = 1;
		NoteCountVal[W] = 1;
		NoteCountVal[F] = 1;
	}

	do {
		//4���[�����ꂼ��̘g�����������v�Z
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//�V�����g���ɓ����Ă����������𑫂�
				if (nc_last[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_last[ind]].group == NoteGroup::Single || note[ind][nc_last[ind]].group == NoteGroup::LongNoteStart)
					&& note[ind][nc_last[ind]].color != NoteColor::K) {//�g�I��肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_last[ind]].color == NoteColor::R ||
						note[ind][nc_last[ind]].color == NoteColor::G ||
						note[ind][nc_last[ind]].color == NoteColor::B) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::Y ||
						note[ind][nc_last[ind]].color == NoteColor::C ||
						note[ind][nc_last[ind]].color == NoteColor::M) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::W) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::F) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//�g������o�Ă�����������������
				if (nc_start[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_start[ind]].group == NoteGroup::Single || note[ind][nc_start[ind]].group == NoteGroup::LongNoteStart)//�g������o�Ă�����������������
					&& note[ind][nc_start[ind]].color != NoteColor::K) {//�g�n�܂肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_start[ind]].color == NoteColor::R ||
						note[ind][nc_start[ind]].color == NoteColor::G ||
						note[ind][nc_start[ind]].color == NoteColor::B) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::Y ||
						note[ind][nc_start[ind]].color == NoteColor::C ||
						note[ind][nc_start[ind]].color == NoteColor::M) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::W) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::F) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		//LocalColor = CalcColor(SlideTime * SlideCount, BoxTime + SlideTime * SlideCount);//�Ǐ�Color�̎Z�o

		//Local���Z�o
		LocalTime = BoxTime;
		if (LocalTime != 0) {
			LocalScoreBuf = ((NoteCount + LocalColor * 0) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}
		SlideCount++;
	} while (SlideCount < CountToSlide);


	/*
	while (NoteCount <= (double)TotalNotes / 10) {//�܂��Ǐ������̉�������K�ȊO�ŉ����T��(1000�m�[�c�Ȃ�100��)
		ind = index(note, nc_last, nc);
		if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
			&& note[ind][nc_last[ind]].color != 8) {//nc���猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

			if (NoteCount == 0)st = note[ind][nc_last[ind]].timing_real;//�ŏ��̉����̃^�C�~���O���i�[
			et = note[ind][nc_last[ind]].timing_real;//�Ō�̉����̃^�C�~���O���i�[
			NoteCount++;
		}
		nc_last[ind]++;//�����珇�ԂɒT��
	}

	//�b��Ǐ����x�v�Z
	LocalTime = et - st;
	if (LocalTime != 0) {
		LocalScore = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
	}

	int find = 0;

	while (NoteCount <= TotalNotes) {//�S�m�[�c�T��������I��

		find = 0;
		while (find == 0) {//�T���̏㑤�@k�ȊO�̒P,LN�n�_��������܂ŒT��
			ind = index(note, nc_last, nc);
			if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
				&& note[ind][nc_last[ind]].color != 8) {//nc���猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�
				et = note[ind][nc_last[ind]].timing_real;//�Ō�̉����̃^�C�~���O���i�[
				NoteCount++;
				find = 1;
			}
			if (nc_last[ind] <= nc[ind]) {//�ő�l�𒴂��Ȃ��悤�ɂ���
				nc_last[ind]++;//�����珇�ԂɒT��
			}
		}

		find = 0;
		while (find == 0) {//�T���̉����@k�ȊO�̒P,LN�n�_��������܂ŒT��
			ind_s = index(note, nc_start, nc);
			if ((note[ind_s][nc_start[ind_s]].group == 0 || note[ind_s][nc_start[ind_s]].group == 1)
				&& note[ind_s][nc_start[ind_s]].color != 8) {//nc���猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�
				st = note[ind_s][nc_start[ind_s]].timing_real;//�Ō�̉����̃^�C�~���O���i�[
				find = 1;
			}
			if (nc_start[ind_s] <= nc[ind_s]) {//�ő�l�𒴂��Ȃ��悤�ɂ���
				nc_start[ind_s]++;//�����珇�ԂɒT��
			}
		}

		LocalTime = et - st;
		if (LocalTime != 0) {
			LocalScoreBuf = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}

	}
	*/

	return (int)(LocalScore * 100 / localMax);
}

int DifficultyRadar::index(NOTE** note, int* nc, int* ncMax) {//�T�����I���Ă��郌�[��(nc>ncMAX)�ɂ��Ă͖�������
	int index = 0;
	int timing = 0;

	if (nc[0] <= ncMax[0]) {
		index = 0;
		timing = note[0][nc[0]].timing;
	}
	else if (nc[1] <= ncMax[1]) {
		index = 1;
		timing = note[1][nc[1]].timing;
	}
	else if (nc[2] <= ncMax[2]) {
		index = 2;
		timing = note[2][nc[2]].timing;
	}
	else if (nc[3] <= ncMax[3]) {
		index = 3;
		timing = note[3][nc[3]].timing;
	}


	for (int i = 1; i <= 3; i++) {
		if (timing > note[i][nc[i]].timing && nc[i] <= ncMax[i]) {
			index = i;
			timing = note[i][nc[i]].timing;
		}
	}
	return index;
}

int DifficultyRadar::CalcColor(int StartTime, int EndTime, bool isRainbow) {//�F��Փx
	double ColorChangeCount = 0;
	double blackDifficulty = 0;
	int lane = 0, NoteCounter = 0;
	NoteColor ColorBuf[4] = { NoteColor::NONE,NoteColor::NONE,NoteColor::NONE,NoteColor::NONE };
	int LocalTime = EndTime - StartTime;

	//�O�̉����̐F�ۑ��p�o�b�t�@
	class colorRingBuf {
	private:
		const int BUF_NUMBER = 6;
		NoteColor colorBuf[4][6];
		int index[4] = { 0,0,0,0 };

		void indexInc(int* index) {
			(*index)++;
			if (*index >= BUF_NUMBER)*index = 0;
		}
		void indexDec(int* index) {
			(*index)--;
			if (*index < 0)*index = BUF_NUMBER - 1;
		}

		double forgettingWeight(int distance) {//�F�Y�p�d��
			switch (distance)
			{
			case 0:
				return 0;
				break;
			case 1:
				return 0.5;
				break;
			case 2:
				return 0.75;
				break;
			case 3:
				return 0.875;
				break;
			case 4:
				return 0.9375;
				break;
			case 5:
				return 1;
				break;
			default:
				return 1;
				break;
			}
		}

	public:
		colorRingBuf() {//�o�b�t�@������
			for (int lane = 0; lane < 4; lane++) {
				for (int ind = 0; ind < BUF_NUMBER; ind++) {
					colorBuf[lane][ind] = NoteColor::NONE;
				}
			}
		}

		void setThisLaneColor(int lane, NoteColor color) {
			colorBuf[lane][index[lane]] = color;
			indexInc(&index[lane]);
		}

		double getThisLaneColorForgettingWeight(int lane, NoteColor color) {//�o�b�t�@�ɓ����F���������T�����F�Y�p�d�݂�Ԃ�
			int searchIndex = index[lane];//�T���n�߂�C���f�b�N�X�ʒu
			indexDec(&searchIndex);//index�͑}������ʒu�Ȃ̂�2�O�̒l�ɂ���
			indexDec(&searchIndex);

			int distance = 0;
			for (distance = 0; distance < BUF_NUMBER - 1; distance++) {
				if (colorBuf[lane][searchIndex] == color) {
					return forgettingWeight(distance);
				}
				indexDec(&searchIndex);
			}
			return forgettingWeight(distance);//������Ȃ������ꍇ�̓f�t�H���g�l��Ԃ�
		}

		double getThisRowColorDifferenceWeight(int lane, NoteColor color) {//�w�肵�������̉��̐F�d�݂�Ԃ�(���߂̉��o�b�t�@�Ƃǂꂾ���F���Ⴄ��)
			int count = 0;
			double weight[4] = {0,0.5,0.75,0.875};
			
			for (int i = 0; i < 4; i++) {
				if (i == lane)continue;

				int ind = index[lane];
				indexDec(&ind);//index�͑}������ʒu�Ȃ̂�1�O�̒l�ɂ���

				if (colorBuf[i][ind] == color) {
					count += 1;
				}
			}

			return 1.0 - weight[count];
		}
	}colorRingBuf;

	ColorBuf[0] = note[0][0].color;
	ColorBuf[1] = note[1][0].color;
	ColorBuf[2] = note[2][0].color;
	ColorBuf[3] = note[3][0].color;
	if (isRainbow) {
		for (int i = 0; i <= 3; i++) {
			if (ColorBuf[i] >= NoteColor::R && ColorBuf[i] <= NoteColor::W) {//�����ȊO�̉����͓��Ƃ��Ĉ���
				ColorBuf[i] = NoteColor::F;
			}
		}
	}
	for (int i = 0; i <= 3; i++) {colorRingBuf.setThisLaneColor(i, ColorBuf[i]);}

	NoteColor NoteColor = NoteColor::NONE;//�����Ă��鉹���̐F
	int k_flag[4] = { 0,0,0,0 };//�O�̉�������������

	serachNotesFromEarly([&](int lane, int index) {
		if (note[lane][index].group == NoteGroup::LongNoteEnd && note[lane][index].LN_k == 1) {//���I�_�̎�
			blackDifficulty += 0.5;
			k_flag[lane] = 1;
		}
		else {//�P�m�[�g��LN�n�_�����ԃm�[�g
			NoteColor = note[lane][index].color;
			if (isRainbow) {
				if (NoteColor >= NoteColor::R && NoteColor <= NoteColor::W) {//�����[�h�͍����ȊO�̉����͓��Ƃ��Ĉ���
					NoteColor = NoteColor::F;
				}
			}

			//���ԁA�I�_�̐F�ω��͏d�݂�ς���
			double weight = (note[lane][index].group == NoteGroup::LongNoteMiddle || note[lane][index].group == NoteGroup::LongNoteEnd) ? 1 : 1;

			if (NoteColor != NoteColor::K) {//k�ȊO
				k_flag[lane] = 0;
			}
			if (ColorBuf[lane] != NoteColor) {//�O�̉����ƐF���Ⴄ
				if (NoteColor != NoteColor::K) {//k�ȊO�Ńo�b�t�@�ɐF�ۑ�
					colorRingBuf.setThisLaneColor(lane, NoteColor);
				}
				if (NoteColor == NoteColor::Y ||
					NoteColor == NoteColor::C ||
					NoteColor == NoteColor::M) {//cmy�Ȃ�4�{�̏d��
					ColorChangeCount += 4 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor) * weight;
					ColorBuf[lane] = NoteColor;
				}
				else if (NoteColor == NoteColor::W) {//w�Ȃ�2�{�̏d��
					ColorChangeCount += 2 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor) * weight;
					ColorBuf[lane] = NoteColor;
				}
				else if (NoteColor == NoteColor::K && k_flag[lane] == 0) {//�O��k�ȊO�ō���k�Ȃ�0.5�{�̏d��
					double distance = note[lane][index].timing_real - note[lane][index - 1].timing_real + 1;
					
					blackDifficulty += 4 / (pow(distance * 0.01, 2) + 1);//�O�̉����ɋ߂��ʒu�ɂ��鍕�m�[�c���d�݂�傫������(�ő�4)
					k_flag[lane] = 1;
				}
				else if (NoteColor == NoteColor::F) {//f�Ȃ�1�{�̏d��
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor) * weight;
					ColorBuf[lane] = NoteColor;
				}
				else if (NoteColor == NoteColor::R ||
					NoteColor == NoteColor::G ||
					NoteColor == NoteColor::B) {//RGB�̏ꍇ��1�{�̏d��
					ColorBuf[lane] = NoteColor;
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor) * weight;
				}
			}
		}


	}); 

	ColorChangeCount = (ColorChangeCount / ((double)LocalTime / 1000)) * 60;
	ColorChangeCount *= 1.725;//�傫������
	//ColorChangeCount = ColorChangeCount / TotalNotesK;//���ʒP�ʂłǂꂾ���F�����G��
	
	//�������̓���̏d�݂��ڗ��悤�ɂ���
	blackDifficulty = (log(blackDifficulty * 0.2 + 1) / log(2)) * 97;
	
	if (isRainbow) {
		//�������Ȃ�
	}
	else {
		//�������̓���̉e���x��1/9�ɂ���
		blackDifficulty = blackDifficulty*0.111;
	}

	double result = ColorChangeCount + blackDifficulty;

	return (int)(result * 100 / colorMax);
}
int DifficultyRadar::CalcLongNote(bool isRainbow) {
	int lane = 0;
	double LN[4] = { 0,0,0,0 };//�e���[����LN�x
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double LNBuf = 0;

	int weight = 1;//�����̐F�ŏd�ݕt�� (RGB:1 CMY:2 W:3)
	int WeightOfColor[3] = { 1,2,3 };//���ꂼ��̉����O���[�v�̏d��

	const int RGBF = 0;
	const int CMY = 1;
	const int W = 2;
	if (isRainbow) {
		WeightOfColor[RGBF] = 1;
		WeightOfColor[CMY] = 1;
		WeightOfColor[W] = 1;
	}

	int noteIndex[4] = { 0,0,0,0 };
	int minTiming = 0;
	double lnDegrees = 0;
	class lnCount {
	private:
		int lnCountEachLane[4] = { 0,0,0,0 };
	public:
		int getLnCountWithoutThisLane(int lane) {
			int lnCount = 0;
			for (int i = 0; i < 4; i++) {
				if (lane != i)lnCount += lnCountEachLane[i];
			}
			return lnCount;
		}
		void setLnCount(int lane, int count) {
			lnCountEachLane[lane] = count;
		}
		void deleteLnCount(int lane) {
			lnCountEachLane[lane] = 0;
		}
		int getLnCountSum() {
			int sum = 0;
			for (int i = 0; i < 4; i++) {
				sum += lnCountEachLane[i];
			}
			return sum;
		}
	}lnCount;

	auto color2weight = [&WeightOfColor, &RGBF, &CMY, &W](NoteColor colorNumber) {
		switch (colorNumber)
		{
		case NoteColor::NONE:
			return 0;
		case NoteColor::R:
			return WeightOfColor[RGBF];
			break;
		case NoteColor::G:
			return WeightOfColor[RGBF];
			break;
		case NoteColor::B:
			return WeightOfColor[RGBF];
			break;
		case NoteColor::Y:
			return WeightOfColor[CMY];
			break;
		case NoteColor::C:
			return WeightOfColor[CMY];
			break;
		case NoteColor::M:
			return WeightOfColor[CMY];
			break;
		case NoteColor::W:
			return WeightOfColor[W];
			break;
		case NoteColor::K:
			return 0;
			break;
		case NoteColor::F:
			return WeightOfColor[RGBF];
			break;
		}
	};

	while (1) {
		//�S�Ẳ�����������I��
		if ((noteIndex[0] == nc[0]) &&
			(noteIndex[1] == nc[1]) &&
			(noteIndex[2] == nc[2]) &&
			(noteIndex[3] == nc[3])) {
			break;
		}

		//�T���I��������[���ȊO�ŉ����ŏ��^�C�~���O�T��
		int first = 1;
		for (lane = 0; lane < 4; lane++) {
			if (noteIndex[lane] < nc[lane]) {
				if (first) {
					minTiming = note[lane][noteIndex[lane]].timing;
					first = 0;
				}
				else if (minTiming > note[lane][noteIndex[lane]].timing)minTiming = note[lane][noteIndex[lane]].timing;
			}
		}

		//�����ŏ��^�C�~���O�Ɠ����m�[�g��T��
		BOOL existNote[4] = { 0,0,0,0 };
		for (lane = 0; lane < 4; lane++) {
			if (note[lane][noteIndex[lane]].timing == minTiming)existNote[lane] = 1;
		}

		//���[������LN��(�F�d�ݕt��)�����Z
		int lnFindCount = 0;//������Z������
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (note[lane][noteIndex[lane]].group == NoteGroup::LongNoteStart || note[lane][noteIndex[lane]].group == NoteGroup::LongNoteMiddle || note[lane][noteIndex[lane]].group == NoteGroup::LongNoteEnd) {
					NoteColor color = note[lane][noteIndex[lane]].color;

					if(note[lane][noteIndex[lane]].group == NoteGroup::LongNoteStart)lnFindCount += color2weight(color);
					lnCount.setLnCount(lane, color2weight(color));
				}
			}
		}

		//LN��(�F�d�ݕt��)��LN�x�ɉ��Z
		lnDegrees += lnFindCount;

		//�e���[���̉����̎w�S�������x������LN�x�ɉ��Z
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				NoteColor color;

				if (note[lane][noteIndex[lane]].LN_k) {//���I�[�Ȃ�1 (�Ԉ���)
					color = NoteColor::R;
				}
				else if (note[lane][noteIndex[lane]].group != NoteGroup::LongNoteEnd) {
					if (note[lane][noteIndex[lane]].group == NoteGroup::LongNoteMiddle) {
						bool isColorDifferent = note[lane][noteIndex[lane]].color != note[lane][noteIndex[lane] - 1].color;
						if (isColorDifferent)continue;
					}

					//���ԃm�[�g�͐F���ς���Ă���ꍇ�̂ݑΏ�
					color = note[lane][noteIndex[lane]].color;
				}
				else {
					bool isColorDifferent = note[lane][noteIndex[lane]].color != note[lane][noteIndex[lane] - 1].color;
					if (isColorDifferent) {
						//�I�_�͐F���ς���Ă���ꍇ�̂ݑΏ�
						color = note[lane][noteIndex[lane]].color;
					}
					else {
						continue;
					}
				}

				//���ԁA�I�[��GLN�̏ꍇ�͏d�݂�ύX
				double weight = 
					note[lane][noteIndex[lane]].group == NoteGroup::LongNoteMiddle ||
					(note[lane][noteIndex[lane]].group == NoteGroup::LongNoteEnd && !note[lane][noteIndex[lane]].LN_k)
					? 1 : 1;
				lnDegrees += weight * sqrt(color2weight(color) * lnCount.getLnCountWithoutThisLane(lane));
			}
		}

		//���[������LN��(�F�d�ݕt��)������
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (note[lane][noteIndex[lane]].group == NoteGroup::LongNoteEnd) {
					lnCount.deleteLnCount(lane);
				}
			}
		}

		//�����̑��݂��郌�[����index���C���N�������g
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (noteIndex[lane] < nc[lane]) {
					noteIndex[lane]++;
				}
			}
		}
	}


	lnDegrees = (lnDegrees / ((double)time / 1000)) * 60;//1���������LN���x�ɂ���
	lnDegrees *= 0.005;
	lnDegrees = (log(lnDegrees + 1) / log(2));//
	lnDegrees *= 2;
	lnDegrees *= 130;

	lnDegrees = lnDegrees * 0.315;//�傫������
	return lnDegrees;
}
int DifficultyRadar::CalcUnstability() {
	//SCROLL�ɂ��u�ԓI�ȉe��
	//STOP�ɂ��u�ԓI�ȉe��
	//���[�����̑��x�ω��X�R�A
	//���Ε��ϕ΍�
	//�����ɎZ�o���܂�

	int BpmCount = 0;
	double Unstability = 0;

	double BPMChangeSum = 0;//BPM�ω���

	int scrollCount = 0;
	double scrollBuf = scrollchange[scrollCount].scroll;
	double scrollChangeSum = 0;//Scroll�ω���

	int lane = 0;
	double HS[4] = { 0,0,0,0 };//�e���[����HS�x
	int NoteCounter[4] = {0,0,0,0};
	bool firstFlag[4] = {false,false,false,false};
	double BpmBuf[4] = { bpmchange[BpmCount].bpm,bpmchange[BpmCount].bpm,bpmchange[BpmCount].bpm,bpmchange[BpmCount].bpm };

	int StopCount = 0;
	double stopSum = 0;

	double OutlierAmount = 0;//���ϑ��x����O�ꂽ�����̓x����

	//���Ε��ϕ΍�(relative_average_deviation)�̎Z�o �����������͕��ςł͂Ȃ�BPM_Suggest���g��
	double mean = std::accumulate(bpmList->begin(), bpmList->end(), 0.0) / bpmList->size();
	double average_deviation = 0; 
	for (const auto &i : *bpmList) {
		average_deviation += abs(i - BPM_suggest);
	}
	
	double relative_average_deviation = average_deviation / mean;

	double rad_weight = 0.025 * relative_average_deviation * TotalNotesRainbow / ((double)time / 1000);//1�b������̑��Ε��ϕ΍��ɂ���

	serachNotesFromEarly([&](int lane, int index) {
		//SCROLL�ɂ��u�ԓI�ȉe���ɂ���
		while (scrollchange[scrollCount+1].use == 1 && (int)(scrollchange[scrollCount+1].timing_real + 0.5) < note[lane][index].timing_real) {//�^���Ⴄ�Ɠ����^�C�~���O�Ɍ덷���o�邽��int�ɃL���X�g���ă^�C�~���O��r
			scrollBuf = scrollchange[scrollCount].scroll;
			scrollCount++;
		}
		if (note[lane][index].color != NoteColor::K) {//���͏��O
			if (((int)(scrollchange[scrollCount].timing_real + 0.5) < note[lane][index].timing_real)
				&& scrollBuf != scrollchange[scrollCount].scroll)
			{
				//Scroll�ω���������
				auto duration = note[lane][index].timing_real - scrollchange[scrollCount].timing_real;  // ���x�ω����Ă��玟�̉����܂ł̃^�C�~���O�Ԋu
				auto weight = pow(1.0 - (duration / 1000), 2);//�Ԋu���Z�����d�݂�傫������ (0~1)
				if (duration >= 1000)weight = 0;//1s�ȏ�Ԋu���J���Ă��疳��

				scrollChangeSum += weight * fabs((log(abs(scrollchange[scrollCount].scroll) + 0.001) / log(2)) - (log(abs(scrollBuf) + 0.001) / log(2))) * 2;
				//2�̑ΐ����Ƃ�Scroll���r �{�̊֌W�ɂȂ��Ă�����1���Z
			}
		}

		//STOP�ɂ��u�ԓI�ȉe���ɂ���
		//��~���Ԃ������A��~���I����Ă��玟�̉����܂ł̊Ԋu���Z����~�̏d�݂�傫������
		while (stopSequence[StopCount+1].use == 1 && (int)(stopSequence[StopCount+1].timing_real + 0.5) < note[lane][index].timing_real) {//�^���Ⴄ�Ɠ����^�C�~���O�Ɍ덷���o�邽��int�ɃL���X�g���ă^�C�~���O��r
			StopCount++;
		}

		if (note[lane][index].color != NoteColor::K) {//���͏��O
			if ((int)(stopSequence[StopCount].timing_real + 0.5) < note[lane][index].timing_real) {
				auto duration = note[lane][index].timing_real - (stopSequence[StopCount].timing_real + stopSequence[StopCount].stop_time * 1000);  // ��~���I����Ă��玟�̉����܂ł̃^�C�~���O�Ԋu
				auto weight = pow(1.0 - (duration / 1000), 2);//�Ԋu���Z�����d�݂�傫������ (0~1)
				if (duration >= 1000)weight = 0;//1s�ȏ�Ԋu���J���Ă��疳��
				stopSum += weight * min(stopSequence[StopCount].stop_time, 1.0) * 9;  // ��~���Ԃ����Z �ő�1�b
			}
		}


		//���[�����̑��x�ω��X�R�A�ɂ���
		if (note[lane][index].color != NoteColor::K) {//���͏��O
			if (firstFlag == false) {
				BpmBuf[lane] = note[lane][index].bpm_real;
				firstFlag[lane] = true;
			}
			else if (BpmBuf[lane] != note[lane][index].bpm_real) {//BPM�ω�(HS�ω�)��������
				HS[lane] += fabs((log(note[lane][index].bpm_real + 0.001) / log(2)) - (log(BpmBuf[lane] + 0.001) / log(2)));
				//2�̑ΐ����Ƃ�BPM���r �{�̊֌W�ɂȂ��Ă�����1���Z
				BpmBuf[lane] = note[lane][index].bpm_real;
			}
		}
	});

	double HS_sum = (HS[0] + HS[1] + HS[2] + HS[3])/2;//BPM�ω��x

	Unstability += scrollChangeSum;
	Unstability += stopSum;
	Unstability += HS_sum;
	Unstability += rad_weight;


	Unstability *= (7.0 / unstabilityMax);//0~7���炢�Ɏ��߂�

	Unstability = (log(Unstability + 1) / log(2));//0~7��0~3�ɑΉ�����ΐ� �ω��������Ȃ������͒l�������オ��悤�ɂ���

	Unstability *= 51.666 * 0.7;

	//Unstability = (Unstability / ((double)time / 1000)) * 60;//1��������̑ΐ�BPM�ω����x�ɂ���
	return (int)(Unstability);
}
int DifficultyRadar::CalcChain() {
	int time_use = time;
	int time_limit = 12000;//����ȉ��̎��ԂŖ��x���o���Ȃ�
	if (time_use <= time_limit)time_use = time_limit;

	int lane = 0;
	double chain[4] = { 0,0,0,0 };//�e���[���̏c�A�x
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double chainBuf = 0;

	int BaseBPM = 150;//16�����c�A�Ƃ݂Ȃ��BPM

	double normalizer = 1.0 / (1000.0 * ((60.0 / BaseBPM) / 4.0));//�BPM�ł�16�������̎��ԊԊu


	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if ((note[lane][NoteCounter].group == NoteGroup::Single || note[lane][NoteCounter].group == NoteGroup::LongNoteStart) && note[lane][NoteCounter].color != NoteColor::K) {
				if (firstFlag == 0) {
					TimingBuf = note[lane][NoteCounter].timing_real;
					firstFlag = 1;
				}
				else {
					chainBuf = (double)1 / pow((((double)note[lane][NoteCounter].timing_real - TimingBuf) * normalizer), 8);//BaseBPM��16�����Z����1����ǂ�ǂ�オ���Ă���
					if (chainBuf >= 1)chainBuf = 1;//1�ȏ�ɂ͂��Ȃ�
					chain[lane] += chainBuf;//
					TimingBuf = note[lane][NoteCounter].timing_real;
				}
			}
		}
	}

	double chainSum = chain[0] + chain[1] + chain[2] + chain[3];
	chainSum = (chainSum / ((double)time_use / 1000)) * 60;//1��������̏c�A���x�ɂ���

	chainSum *= 0.07992;//316��12�Ɏ��߂�
	chainSum = (log(chainSum + 1) / log(2));//12��3.5�Ɉ��k�����

	chainSum *= 49.723;//�傫������

	return (int)(chainSum * 100 / chainMax);
}

void DifficultyRadar::serachNotesFromEarly(function<void(int lane, int index)> handler)
{
	int noteIndex[4] = { 0,0,0,0 };
	int lane = 0;
	int minTiming = 0;

	while (1) {
		//�S�Ẳ�����������I��
		if ((noteIndex[0] == nc[0]) &&
			(noteIndex[1] == nc[1]) &&
			(noteIndex[2] == nc[2]) &&
			(noteIndex[3] == nc[3])) {
			break;
		}

		//�T���I��������[���ȊO�ŉ����ŏ��^�C�~���O�T��
		int first = 1;
		for (lane = 0; lane < 4; lane++) {
			if (noteIndex[lane] < nc[lane]) {
				if (first) {
					minTiming = note[lane][noteIndex[lane]].timing;
					first = 0;
				}
				else if (minTiming > note[lane][noteIndex[lane]].timing)minTiming = note[lane][noteIndex[lane]].timing;
			}
		}

		//�����ŏ��^�C�~���O�Ɠ����m�[�g��T��
		BOOL existNote[4] = { 0,0,0,0 };
		for (lane = 0; lane < 4; lane++) {
			if (note[lane][noteIndex[lane]].timing == minTiming) {
				existNote[lane] = 1;
				handler(lane, noteIndex[lane]);
			}
		}

		

		//�����̑��݂��郌�[����index���C���N�������g
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (noteIndex[lane] < nc[lane]) {
					noteIndex[lane]++;
				}
			}
		}
	}
}
