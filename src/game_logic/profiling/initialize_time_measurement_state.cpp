#include "game_state/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void initialize_time_measurement_state(::game_environment::Environment& environment)
	{
		environment.state.profiling.time_measurement = new ::game_state::profiling::Time_Measurement_State();
		environment.state.profiling.time_measurement->timestamp_capacity = 1000000u;
		
		environment.state.profiling.time_measurement->measured_type_count = 0u;
		for (GLuint type{ 0u }; type < ::game_state::profiling::Timestamp_Type::count; ++type)
		{
			environment.state.profiling.time_measurement->measured_types[type] = false;
		}

		environment.state.profiling.time_measurement->next = 0u;

		glCreateBuffers(1u, &environment.state.profiling.time_measurement->buffer);
		glNamedBufferStorage
		(
			environment.state.profiling.time_measurement->buffer,
			environment.state.profiling.time_measurement->timestamp_capacity * ::game_state::profiling::timestamp_value_size,
			nullptr,
			0
		);

		environment.state.profiling.time_measurement->timestamp_metadata = new ::game_state::profiling::Timestamp_Metadata[environment.state.profiling.time_measurement->timestamp_capacity];
	}
}
