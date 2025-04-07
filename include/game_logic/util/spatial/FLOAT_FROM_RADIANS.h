#pragma once
#include "game_logic/util/spatial/FLOAT_FROM_UNIT_VALUE.h"
#include "game_logic/util/spatial/RADIAN.h"

#define game_logic__util__spatial_FLOAT_FROM_RADIANS(environment, radians) \
	game_logic__util__spatial_FLOAT_FROM_UNIT_VALUE\
	(\
		radians, game_logic__util__spatial_RADIAN(environment)\
	\)
