#include<math.h>
#include"xtosinx.h"

double xtosinx(double x) {//0~100のxをsin的に歪ませる
	return ((double)1 - cos((3.14159265 / 2) * (x / (double)100))) * 100;
}