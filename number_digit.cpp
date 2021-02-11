#include<math.h>
#include"number_digit.h"

void number_digit(int number, int digit[], int n) {//数値を桁数毎に配列に格納(n:配列の要素数)
	int i = 0;
	for (i = 0; i <= n - 1; i++) {//コンボ数を桁数毎に格納
		digit[i] = int((number % int(pow(10, i + 1))) / pow(10, i));
	}

}