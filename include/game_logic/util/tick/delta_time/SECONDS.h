#pragma once
#include "game_logic/util/tick/delta_time/TICKS_PER_SECOND.h"

#define game_logic__util__tick__delta_time_SECONDS(environment) \
	1.0f / game_logic__util__tick__delta_time_TICKS_PER_SECOND(environment)
