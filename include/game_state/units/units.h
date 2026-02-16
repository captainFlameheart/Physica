#pragma once
#include "glad_glfw.h"

namespace game_state::units
{
	constexpr GLfloat meter_in_length_units{ 1000.0f };
	constexpr GLfloat length_unit_in_meters{ 1.0f / meter_in_length_units };

	constexpr GLfloat second_in_time_units{ 60.0f };
	constexpr GLfloat time_unit_in_seconds{ 1.0f / second_in_time_units };

	constexpr GLfloat meters_per_second_in_length_units_per_time_unit{ meter_in_length_units / second_in_time_units };

	constexpr GLfloat meters_per_second_squared_in_length_units_per_time_unit_squared{ meter_in_length_units / (second_in_time_units * second_in_time_units) };
}
