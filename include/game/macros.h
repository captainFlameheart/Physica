#pragma once
#include <cmath>

#define game_INITIAL_TICK 100
#define game_TICK(environment) game_INITIAL_TICK
#define game_MAX_TICKS_PER_FRAME(environment) 10u

#define game_METER(environment) 1000.0
#define game_RADIAN(environment) 1000000.0
#define game_KILOGRAM(environment) 1000.0
#define game_SECOND(environment) (120.0 * game_INITIAL_TICK)

#define game_FROM_UNIT_VALUE(value, unit) static_cast<GLint>(std::round(value * unit))

#define game_FROM_METERS(environment, meters) \
	game_FROM_UNIT_VALUE(meters, game_METER(environment))

#define game_FROM_RADIANS(environment, radians) \
	game_FROM_UNIT_VALUE(radians, game_RADIAN(environment))

#define game_FROM_KILOGRAMS(environment, kilograms) \
	game_FROM_UNIT_VALUE(kilograms, game_KILOGRAM(environment))

#define game_FROM_SECONDS(environment, seconds) \
	game_FROM_UNIT_VALUE(seconds, game_SECOND(environment))

#define game_TO_UNIT_VALUE(value, unit) value / unit

#define game_TO_METERS(environment, length) \
	game_TO_UNIT_VALUE(value, game_METER(environment))

#define game_TO_RADIANS(environment, angle) \
	game_TO_UNIT_VALUE(value, game_RADIAN(environment))

#define game_TO_KILOGRAMS(environment, mass) \
	game_TO_UNIT_VALUE(value, game_KILOGRAM(environment))

#define game_TO_SECONDS(environment, time) \
	game_TO_UNIT_VALUE(value, game_SECOND(environment))
