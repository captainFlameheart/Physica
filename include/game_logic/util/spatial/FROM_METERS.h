#pragma once
#include "game_logic/util/spatial/FROM_UNIT_VALUE.h"
#include "game_logic/util/spatial/METER.h"

#define game_logic__util__spatial_FROM_METERS(environment, meters) \
	game_logic__util__spatial_FROM_UNIT_VALUE\
	(\
		meters, game_logic__util__spatial_METER(environment)\
	)
