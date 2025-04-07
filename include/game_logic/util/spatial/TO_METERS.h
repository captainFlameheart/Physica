#pragma once
#include "game_logic/util/spatial/TO_UNIT_VALUE.h"
#include "game_logic/util/spatial/METER_INVERSE.h"

#define game_logic__util__spatial_TO_METERS(environment, length) \
	game_logic__util__spatial_TO_UNIT_VALUE\
	(\
		length, game_logic__util__spatial_METER_INVERSE(environment)\
	)
