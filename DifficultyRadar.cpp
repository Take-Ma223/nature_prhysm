#include "DifficultyRadar.h"
#include<math.h>

DifficultyRadar::DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE* stopSequence, SC* scrollchange, int time, int StartTime, int EndTime, int* TimingSame, short* NotesAmount, double BPM_suggest) {
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

	int i = 0;
	for (i = 0; i < 9; i++) {//�e�F�������̏�����
		NotesAmount[i] = 0;
	}

	int lane = 0, NoteCounter = 0;
	//�����̍��v���擾
	for (lane = 0; lane <= 3; lane++) {
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1) {
				if (note[lane][NoteCounter].color != 8) {

					if (note[lane][NoteCounter].color == 1 ||
						note[lane][NoteCounter].color == 2 ||
						note[lane][NoteCounter].color == 3) {//RGB
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 4 ||
						note[lane][NoteCounter].color == 5 ||
						note[lane][NoteCounter].color == 6) {//CMY
						TotalNotes += 2;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 7) {//W
						TotalNotes += 3;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 9) {//F
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
				}
				TotalNotesK++;
				NotesAmount[NumberTranslation(note[lane][NoteCounter].color)]++;//�e�F���������邩���Z
			}
			if (note[lane][NoteCounter].group == 2 && note[lane][NoteCounter].LN_k == 1) {//LN�I�[�̍�
				NotesAmount[NumberTranslation(8)]++;//�e�F���������邩���Z
			}
		}
	}

	for (i = 0; i < 9; i++) {//�e�F�������𕽋ϖ��x�ɂ���
		NotesAmount[i] = int(200 * log10((double)NotesAmount[i] * 30000 / (DifficultyRadar::time)+1));
	}

}

void DifficultyRadar::GetLocalNotesGraph(short* LocalNotesGraph) {
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

	do {
		//4���[�����ꂼ��̘g�����������v�Z
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//�V�����g���ɓ����Ă����������𑫂�
				if (nc_last[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != 8) {//�g�I��肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_last[ind]].color == 1 ||
						note[ind][nc_last[ind]].color == 2 ||
						note[ind][nc_last[ind]].color == 3) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == 4 ||
						note[ind][nc_last[ind]].color == 5 ||
						note[ind][nc_last[ind]].color == 6) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == 7) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == 9) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//�g������o�Ă�����������������
				if (nc_start[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//�g������o�Ă�����������������
					&& note[ind][nc_start[ind]].color != 8) {//�g�n�܂肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_start[ind]].color == 1 ||
						note[ind][nc_start[ind]].color == 2 ||
						note[ind][nc_start[ind]].color == 3) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == 4 ||
						note[ind][nc_start[ind]].color == 5 ||
						note[ind][nc_start[ind]].color == 6) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == 7) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == 9) {//F
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

int DifficultyRadar::NumberTranslation(int color) {
	int val = color;

	switch (val)
	{
	default:
		break;
	case 4:
		val = 6;//y
		break;
	case 5:
		val = 4;//y
		break;
	case 6:
		val = 5;//y
		break;
	}

	val--;//0~8�ɂ���

	return val;
}


int DifficultyRadar::CalcGlobal(int Rainbow) {
	double GlobalScore = 0;

	if (Rainbow == 0) {
		GlobalScore = TotalNotes;
	}
	else if (Rainbow == 1) {
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

int DifficultyRadar::CalcLocal(int Rainbow) {
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
	if (Rainbow == 1) {
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

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != 8) {//�g�I��肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_last[ind]].color == 1 ||
						note[ind][nc_last[ind]].color == 2 ||
						note[ind][nc_last[ind]].color == 3) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == 4 ||
						note[ind][nc_last[ind]].color == 5 ||
						note[ind][nc_last[ind]].color == 6) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == 7) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == 9) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//�g������o�Ă�����������������
				if (nc_start[ind] > nc[ind]) {//���̃��[���̍ő�l�𒴂��Ă�����I��
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//�g������o�Ă�����������������
					&& note[ind][nc_start[ind]].color != 8) {//�g�n�܂肩�猩�Ĉ�ԉ��ɂ���P,LN�n�_�m�[�g��K����Ȃ�

					if (note[ind][nc_start[ind]].color == 1 ||
						note[ind][nc_start[ind]].color == 2 ||
						note[ind][nc_start[ind]].color == 3) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == 4 ||
						note[ind][nc_start[ind]].color == 5 ||
						note[ind][nc_start[ind]].color == 6) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == 7) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == 9) {//F
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

int DifficultyRadar::CalcColor(int StartTime, int EndTime, int Rainbow) {//�F��Փx
	double ColorChangeCount = 0;
	int lane = 0, NoteCounter = 0;
	int ColorBuf[4] = { 0,0,0,0 };
	int LocalTime = EndTime - StartTime;

	//�O�̉����̐F�ۑ��p�o�b�t�@
	class colorRingBuf {
	private:
		const int BUF_NUMBER = 6;
		int colorBuf[4][6];
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
					colorBuf[lane][ind] = 0;
				}
			}
		}

		void setThisLaneColor(int lane, int color) {
			colorBuf[lane][index[lane]] = color;
			indexInc(&index[lane]);
		}

		double getThisLaneColorForgettingWeight(int lane, int color) {//�o�b�t�@�ɓ����F���������T�����F�Y�p�d�݂�Ԃ�
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

		double getThisRowColorDifferenceWeight(int lane, int color) {//�w�肵�������̉��̐F�d�݂�Ԃ�(���߂̉��o�b�t�@�Ƃǂꂾ���F���Ⴄ��)
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
	if (Rainbow == 1) {
		for (int i = 0; i <= 3; i++) {
			if (ColorBuf[i] >= 1 && ColorBuf[i] <= 7) {//�����ȊO�̉����͓��Ƃ��Ĉ���
				ColorBuf[i] = 9;
			}
		}
	}
	for (int i = 0; i <= 3; i++) {colorRingBuf.setThisLaneColor(i, ColorBuf[i]);}

	int NoteColor = 0;//�����Ă��鉹���̐F
	int k_flag = 0;//�O�̉�������������

	serachNotesBFS([&](int lane, int index) {
		if ((note[lane][index].group == 0 || note[lane][index].group == 1)) {//�P�m�[�g��LN�n�_
			NoteColor = note[lane][index].color;
			if (Rainbow == 1) {
				if (NoteColor >= 1 && NoteColor <= 7) {//�����[�h�͍����ȊO�̉����͓��Ƃ��Ĉ���
					NoteColor = 9;
				}
			}

			if (ColorBuf[lane] != NoteColor) {//�O�̉����ƐF���Ⴄ
				if (NoteColor != 8) {//k�ȊO�Ńo�b�t�@�ɐF�ۑ�
					colorRingBuf.setThisLaneColor(lane, NoteColor);
				}
				if (NoteColor == 4 ||
					NoteColor == 5 ||
					NoteColor == 6) {//cmy�Ȃ�4�{�̏d��
					ColorChangeCount += 4 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == 7) {//w�Ȃ�2�{�̏d��
					ColorChangeCount += 2 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == 8 && k_flag == 0) {//�O��k�ȊO�ō���k�Ȃ�0.5�{�̏d��
					ColorChangeCount += 0.5;
					k_flag = 1;
				}
				else if (NoteColor == 9) {//f�Ȃ�1�{�̏d��
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == 1 ||
					NoteColor == 2 ||
					NoteColor == 3) {//RGB�̏ꍇ��1�{�̏d��
					ColorBuf[lane] = NoteColor;
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					k_flag = 0;
				}
			}
		}
		else if (note[lane][index].group == 2 && note[lane][index].LN_k == 1) {//���I�_�̎�
			ColorChangeCount += 0.5;
			k_flag = 1;
		}

	});

	ColorChangeCount = (ColorChangeCount / ((double)LocalTime / 1000)) * 60;
	ColorChangeCount *= 1.725;//�傫������
	//ColorChangeCount = ColorChangeCount / TotalNotesK;//���ʒP�ʂłǂꂾ���F�����G��

	return (int)(ColorChangeCount * 100 / colorMax);
}
int DifficultyRadar::CalcLongNote(int Rainbow) {
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
	if (Rainbow == 1) {
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

	auto color2weight = [](int colorNumber) {
		switch (colorNumber)
		{
		case 0:
			return 0;
		case 1:
			return 1;
			break;
		case 2:
			return 1;
			break;
		case 3:
			return 1;
			break;
		case 4:
			return 2;
			break;
		case 5:
			return 2;
			break;
		case 6:
			return 2;
			break;
		case 7:
			return 3;
			break;
		case 8:
			return 0;
			break;
		case 9:
			return 1;
		case 10:
			return 0;
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
				if (note[lane][noteIndex[lane]].group == 1) {
					int color = note[lane][noteIndex[lane]].color;
					lnFindCount += color2weight(color);
					lnCount.setLnCount(lane, color2weight(color));
				}
			}
		}

		//LN��(�F�d�ݕt��)��LN�x�ɉ��Z
		lnDegrees += lnFindCount;

		//�e���[���̉����̎w�S�������x������LN�x�ɉ��Z
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				int color;

				if (note[lane][noteIndex[lane]].LN_k) {//���I�[�Ȃ�1 (�Ԉ���)
					color = 1;
				}
				else if (note[lane][noteIndex[lane]].group != 2) {
					color = note[lane][noteIndex[lane]].color;
				}
				else {//�ʏ�̏I�_�͏��O
					continue;
				}

				lnDegrees += sqrt(color2weight(color) * lnCount.getLnCountWithoutThisLane(lane));
			}
		}

		//���[������LN��(�F�d�ݕt��)������
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (note[lane][noteIndex[lane]].group == 2) {
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
	//BPM�ɂ��ĎZ�o
	int BpmCount = 0;
	double Unstability = 0;

	BpmCount = 0;
	double BpmBuf = bpmchange[BpmCount].bpm;
	double BPMChangeSum = 0;//BPM�ω���

	int scrollCount = 0;
	double scrollBuf = scrollchange[scrollCount].scroll;
	double scrollChangeSum = 0;//Scroll�ω���

	/*
	while (bpmchange[BpmCount].use == 1) {
		if (BpmBuf != bpmchange[BpmCount].bpm) {//BPM�ω���������
			BPMChangeSum += fabs((log(bpmchange[BpmCount].bpm + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2))) * 2;//BPM�ω��̏d�݂�2�{
			//2�̑ΐ����Ƃ�BPM���r �{�̊֌W�ɂȂ��Ă�����1���Z
			BpmBuf = bpmchange[BpmCount].bpm;
		}
		BpmCount++;
	}
	BPMChangeSum = 0;//�s�g�p
	*/


	//ScrollChange�ɂ��ĎZ�o
	while (scrollchange[scrollCount].use == 1) {
		if (scrollBuf != scrollchange[scrollCount].scroll) {//Scroll�ω���������
			scrollChangeSum += fabs((log(abs(scrollchange[scrollCount].scroll) + 0.1) / log(2)) - (log(abs(scrollBuf) + 0.1) / log(2))) * 2;//Scroll�ω��̏d�݂�2�{
			//2�̑ΐ����Ƃ�Scroll���r �{�̊֌W�ɂȂ��Ă�����1���Z
			scrollBuf = scrollchange[scrollCount].scroll;
		}
		scrollCount++;
	}
	Unstability += scrollChangeSum;


	//Stop�ɂ��ĎZ�o
	int StopCount = 0;
	double stopSum = 0;

	while (stopSequence[StopCount].use == 1) {
		stopSum += fabs(stopSequence[StopCount].stop_time);//��~���Ԃ����Z
		StopCount++;
	}

	Unstability += stopSum * 0.5 + (double)StopCount * 4;//BPM,HS�Ƃ̏d�݃o�����X����


	//HS,�u�ԕ������痣�ꂽ�����̉����ɂ��ĎZ�o
	int lane = 0;
	double HS[4] = { 0,0,0,0 };//�e���[����HS�x
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	BpmBuf = 0;

	double OutlierAmount = 0;//�u�ԕ�������O�ꂽ�����̓x����

	//���[�����̉������x�ω�������
	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].color != 8) {//���͏��O
				//HS,BPM�ω��ɂ���
				if (firstFlag == 0) {
					BpmBuf = note[lane][NoteCounter].bpm_real;
					firstFlag = 1;
				}
				else if (BpmBuf != note[lane][NoteCounter].bpm_real) {//BPM�ω�(HS�ω�)��������
					HS[lane] += fabs((log(note[lane][NoteCounter].bpm_real + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2)));
					//2�̑ΐ����Ƃ�BPM���r �{�̊֌W�ɂȂ��Ă�����1���Z
					BpmBuf = note[lane][NoteCounter].bpm_real;
				}

				//�u�ԕ�������O�ꂽ�����̓x�����ɂ���
				OutlierAmount += abs(log((note[lane][NoteCounter].bpm_real + 0.1) / (BPM_suggest + 0.1)) / log(2));//�u�ԕ�������O�ꂽ�����̉����ɏd�݂�t���ĉ��Z
			}
		}
	}

	double HS_sum = (HS[0] + HS[1] + HS[2] + HS[3]);//BPM�ω��x

	//�l�̑傫���𒲐�
	OutlierAmount = OutlierAmount * 16 / ((double)time / 1000);
	//��b������ɂǂꂾ���d�ݕt���u�ԕ����O�ꉹ�������邩���v�Z
	//OutlierAmount = OutlierAmount / ((double)time / 1000);

	Unstability += OutlierAmount;//�d�ݕt���u�ԕ����O�ꉹ�����x�𑫂�

	Unstability += HS_sum;//BPM�ω��ƒނ荇�����Ƃ邽��4�Ŋ���

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
			if ((note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1) && note[lane][NoteCounter].color != 8) {
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

void DifficultyRadar::serachNotesBFS(function<void(int lane, int index)> handler)
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