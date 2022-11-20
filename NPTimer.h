#pragma once
#include "STRUCT_CONFIG.h"
#include <cstddef>

class NPTimer
{
	Config* config;

public:
	NPTimer(Config* config = nullptr);

	double getNowCount();

};

