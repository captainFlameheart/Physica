#pragma once
#include "game_logic/util/spatial/FLOAT_FROM_UNIT_VALUE.h"

#define game_logic__util__spatial_FROM_UNIT_VALUE(value, unit) \
	static_cast<GLint>(\
		game_logic__util__spatial_FLOAT_FROM_UNIT_VALUE(value, unit)\
	)
