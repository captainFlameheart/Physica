#pragma once
#include "game_logic/util/spatial/UNIT_INVERSE.h"
#include "game_logic/util/spatial/METER.h"

#define game_logic__util__spatial_METER_INVERSE(environment) \
	game_logic__util__spatial_UNIT_INVERSE(game_logic__util__spatial_METER(environment))
