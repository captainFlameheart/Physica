#pragma once
#include "game_logic/util/projection/INVERSE_SCALE_Y.h"

#define game_logic__util__projection_SCALE_Y(environment) \
	1.0f / game_logic__util__projection_INVERSE_SCALE_Y(environment)
