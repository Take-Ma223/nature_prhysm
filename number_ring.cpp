#include"number_ring.h"

int number_ring(int number, int ring_number) {//0~ring_numberの範囲で値を回す

	while (number > ring_number) {
		number = number - ring_number - 1;
	}
	while (number < 0) {
		number = ring_number + number + 1;
	}

	return number;
}