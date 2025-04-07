#pragma once
#include "game_logic/util/spatial/TO_UNIT_VALUE.h"
#include "game_logic/util/spatial/RADIAN_INVERSE.h"

#define game_logic__util__spatial_TO_RADIANS(environment, angle) \
	game_logic__util__spatial_TO_UNIT_VALUE\
	(\
		angle, game_logic__util__spatial_RADIAN_INVERSE(environment)\
	)
