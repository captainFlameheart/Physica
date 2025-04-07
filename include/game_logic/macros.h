#pragma once
#include <cmath>
#include "macros/macros.h"
#include "util/shader/shader.h"
#include "game_logic/util/tick/delta_time/SECONDS.h"
#include "game_logic/util/tick/MAX_TICKS_PER_FRAME.h"

#define game_METER(environment) 1000000.0f
#define game_RADIAN(environment) 1000000.0f

#define game_UNIT_INVERSE(unit) 1.0f / unit

#define game_METER_INVERSE(environment) game_UNIT_INVERSE(game_METER(environment))
#define game_RADIAN_INVERSE(environment) game_UNIT_INVERSE(game_RADIAN(environment))

#define game_FLOAT_FROM_UNIT_VALUE(value, unit) value * unit

#define game_FLOAT_FROM_METERS(environment, meters) \
	game_FLOAT_FROM_UNIT_VALUE(meters, game_METER(environment))

#define game_FLOAT_FROM_RADIANS(environment, radians) \
	game_FLOAT_FROM_UNIT_VALUE(radians, game_RADIAN(environment))

#define game_FROM_UNIT_VALUE(value, unit) \
	static_cast<GLint>(game_FLOAT_FROM_UNIT_VALUE(value, unit))

#define game_FROM_METERS(environment, meters) \
	game_FROM_UNIT_VALUE(meters, game_METER(environment))

#define game_FROM_RADIANS(environment, radians) \
	game_FROM_UNIT_VALUE(radians, game_RADIAN(environment))

#define game_FROM_SECONDS(environment, seconds) \
	game_FROM_UNIT_VALUE(seconds, game_SECOND(environment))

#define game_TO_UNIT_VALUE(value, unit_inverse) value * unit_inverse

#define game_TO_METERS(environment, length) \
	game_TO_UNIT_VALUE(length, game_METER_INVERSE(environment))

#define game_TO_RADIANS(environment, angle) \
	game_TO_UNIT_VALUE(angle, game_RADIAN_INVERSE(environment))

#define game_TO_KILOGRAMS(environment, mass) \
	game_TO_UNIT_VALUE(mass, game_KILOGRAM_INVERSE(environment))

#define game_TO_SECONDS(environment, time) \
	game_TO_UNIT_VALUE(time, game_SECOND_INVERSE(environment))

#define game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, meters_per_second) \
	meters_per_second * game_METER(environment) * game_logic__util__tick__delta_time_SECONDS(environment)

#define game_METERS_PER_SECOND_TO_LENGTH_PER_TICK(environment, meters_per_second) \
	static_cast<GLint>(game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, meters_per_second))

#define game_RADIANS_PER_SECOND_TO_FLOAT_ANGLE_PER_TICK(environment, radians_per_second) \
	radians_per_second * game_RADIAN(environment) * game_logic__util__tick__delta_time_SECONDS(environment)

#define game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, radians_per_second) \
	static_cast<GLint>(game_RADIANS_PER_SECOND_TO_FLOAT_ANGLE_PER_TICK(environment, radians_per_second))

#define game_INVERSE_PROJECTION_SCALE_X(environment) 2.0f
#define game_INVERSE_PROJECTION_SCALE_Y(environment) 1.0f
#define game_PROJECTION_SCALE_X(environment) 1.0f / game_INVERSE_PROJECTION_SCALE_X(environment)
#define game_PROJECTION_SCALE_Y(environment) 1.0f / game_INVERSE_PROJECTION_SCALE_Y(environment)
#define game_PROJECTION_SCALE_DEFINITION(environment) \
	util_shader_DEFINE\
	(\
		"PROJECTION_SCALE", \
		"vec2" \
		"("\
			STRINGIFY(game_PROJECTION_SCALE_X(environment)) ", " \
			STRINGIFY(game_PROJECTION_SCALE_Y(environment))\
		")"\
	)

#define game_CAMERA_BINDING 0

#define game_CAMERA_DEFAULT_DISTANCE_PER_TICK(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 5.0f)

#define game_CAMERA_FAST_DISTANCE_PER_TICK_INCREASE(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 5.0f)

#define game_CAMERA_SLOW_DISTANCE_PER_TICK_DECREASE(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 4.0f)

#define game_CAMERA_DEFAULT_Z_DISTANCE_PER_TICK(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 5.0f)

#define game_CAMERA_FAST_Z_DISTANCE_PER_TICK_INCREASE(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 15.0f)

#define game_CAMERA_SLOW_Z_DISTANCE_PER_TICK_DECREASE(environment) \
	game_METERS_PER_SECOND_TO_FLOAT_LENGTH_PER_TICK(environment, 4.0f)

#define game_CAMERA_DEFAULT_ANGLE_PER_TICK(environment) \
	game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 2.0f)

#define game_CAMERA_FAST_ANGLE_PER_TICK_INCREASE(environment) \
	game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 2.0f)

#define game_CAMERA_SLOW_ANGLE_PER_TICK_DECREASE(environment) \
	game_RADIANS_PER_SECOND_TO_ANGLE_PER_TICK(environment, 1.5f)

#define game_CAMERA_SCROLL_ZOOM_DISTANCE(environment) \
	game_FLOAT_FROM_METERS(environment, 0.5f)
#define game_CAMERA_SCROLL_ANGLE(environment) \
	game_FROM_RADIANS(environment, 0.3f)
