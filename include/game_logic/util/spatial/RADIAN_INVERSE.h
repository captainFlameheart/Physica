#pragma once
#include "game_logic/util/spatial/UNIT_INVERSE.h"
#include "game_logic/util/spatial/RADIAN.h"

#define game_logic__util__spatial_RADIAN_INVERSE(environment) \
	game_logic__util__spatial_UNIT_INVERSE(game_logic__util__spatial_RADIAN(environment))
