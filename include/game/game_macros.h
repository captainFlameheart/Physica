#pragma once
#include "full_game_state.h"
#include <cmath>

#define game_INITIAL_TICK 100
#define game_TICK(game_state) game_INITIAL_TICK
#define game_MAX_TICKS_PER_FRAME(game_state) 10u

#define game_METER(game_state) 1000.0
#define game_RADIAN(game_state) 1000000.0
#define game_KILOGRAM(game_state) 1000.0
#define game_SECOND(game_state) (120.0 * game_INITIAL_TICK)

#define game_FROM_UNIT_VALUE(value, unit) static_cast<GLint>(std::round(value * unit))

#define game_FROM_METERS(game_state, meters) \
	game_FROM_UNIT_VALUE(meters, game_METER(game_state))

#define game_FROM_RADIANS(game_state, radians) \
	game_FROM_UNIT_VALUE(radians, game_RADIAN(game_state))

#define game_FROM_KILOGRAMS(game_state, kilograms) \
	game_FROM_UNIT_VALUE(kilograms, game_KILOGRAM(game_state))

#define game_FROM_SECONDS(game_state, seconds) \
	game_FROM_UNIT_VALUE(seconds, game_SECOND(game_state))

#define game_TO_UNIT_VALUE(value, unit) value / unit

#define game_TO_METERS(game_state, length) \
	game_TO_UNIT_VALUE(value, game_METER(game_state))

#define game_TO_RADIANS(game_state, angle) \
	game_TO_UNIT_VALUE(value, game_RADIAN(game_state))

#define game_TO_KILOGRAMS(game_state, mass) \
	game_TO_UNIT_VALUE(value, game_KILOGRAM(game_state))

#define game_TO_SECONDS(game_state, time) \
	game_TO_UNIT_VALUE(value, game_SECOND(game_state))
