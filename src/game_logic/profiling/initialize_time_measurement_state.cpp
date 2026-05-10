#include "game_state/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void initialize_time_measurement_state(::game_environment::Environment& environment)
	{
		environment.state.profiling.time_measurement = new ::game_state::profiling::Time_Measurement_State();
		environment.state.profiling.time_measurement->timestamp_capacity = 1000000u;
		environment.state.profiling.time_measurement->query_capacity = 1000u;
		
		environment.state.profiling.time_measurement->measured_type_count = 0u;
		for (GLuint type{ 0u }; type < ::game_state::profiling::Timestamp_Type::count; ++type)
		{
			environment.state.profiling.time_measurement->measured_types[type] = false;
			environment.state.profiling.time_measurement->previous_timestamps[type] = null_uint;
		}

		environment.state.profiling.time_measurement->timestamp_base = 0u;
		environment.state.profiling.time_measurement->next = 0u;
		environment.state.profiling.time_measurement->base_query = 0u;
		environment.state.profiling.time_measurement->query_count = 0u;

		glCreateBuffers(1u, &environment.state.profiling.time_measurement->buffer);
		glNamedBufferStorage
		(
			environment.state.profiling.time_measurement->buffer,
			environment.state.profiling.time_measurement->timestamp_capacity * ::game_state::profiling::timestamp_value_size,
			nullptr,
			0
		);
		glBindBuffer(GL_QUERY_BUFFER, environment.state.profiling.time_measurement->buffer);

		environment.state.profiling.time_measurement->queries = new GLuint[environment.state.profiling.time_measurement->query_capacity];
		glCreateQueries(GL_TIMESTAMP, environment.state.profiling.time_measurement->timestamp_capacity, environment.state.profiling.time_measurement->queries);

		environment.state.profiling.time_measurement->timestamp_metadata = new ::game_state::profiling::Timestamp_Metadata[environment.state.profiling.time_measurement->timestamp_capacity];

		for (GLuint timestamp{ 0u }; timestamp < environment.state.profiling.time_measurement->timestamp_capacity; ++timestamp)
		{
			environment.state.profiling.time_measurement->timestamp_metadata[timestamp].next = timestamp;
			environment.state.profiling.time_measurement->timestamp_metadata[timestamp].name = "";
		}
	}
}
