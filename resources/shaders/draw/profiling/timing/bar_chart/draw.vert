float gl_CullDistance[1u];

const uint vertices_per_timestamp = 6u;

const vec2 bar_vertices[] =
{
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, 0.0),

	vec2(-1.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
};

flat out vec4 timestamp_color;

const float millisecond_in_nanoseconds = 1000000.0;
const float nanosecond_in_milliseconds = 1.0 / millisecond_in_nanoseconds;

void main()
{
	const float chart_base = -1.0;
	const float bar_half_width = 0.01;
	const float deadline_height = 0.5;
	const float deadline_milliseconds = 1000.0 / 60.0;
	const float millisecond_height = deadline_height / deadline_milliseconds;
	const float nanosecond_height = nanosecond_in_milliseconds * millisecond_height;

	uint timestamp_index = gl_VertexID / vertices_per_timestamp;
	uint local_index = gl_VertexID - timestamp_index * vertices_per_timestamp;

	uint timestamp_capacity = timing_configuration.timestamp_capacity;
	uint metadata_stage_capacity = timing_configuration.metadata_stage_capacity;
	uint full_timestamp_metadata_capacity = timestamp_capacity + metadata_stage_capacity;
	uint generation_capacity = timing_configuration.generation_capacity;

	uint next_generation = timing_metadata.generation;
	uint next_timestamp = timing_metadata.next_timestamp;

	if (timestamp_index >= next_timestamp)
	{
		gl_CullDistance[0u] = -1.0;
		return;
	}
	timestamp_index = next_timestamp - 1u - timestamp_index;

	uint oldest_generation = next_generation - min(generation_capacity, next_generation);
	uint oldest_timestamp = next_timestamp - min(timestamp_capacity, next_timestamp);

	if (timestamp_index <= oldest_timestamp && oldest_timestamp != 0u)
	{
		gl_CullDistance[0u] = -1.0;
		return;
	}
	
	uint physical_timestamp_index = timestamp_index % timestamp_capacity;
	uint physical_metadata_index = timestamp_index % full_timestamp_metadata_capacity;

	uint64_t timestamp_value = timestamps.timestamps[physical_timestamp_index];
	_Timestamp_Metadata metadata = timestamp_metadata.metadata[physical_metadata_index];

	if (metadata.generation < oldest_generation)
	{
		gl_CullDistance[0u] = -1.0;
		return;
	}

	uint64_t generation_timestamp_value = timestamps.timestamps[timestamp_capacity + (metadata.generation % generation_capacity)];

	uint64_t previous_timestamp_value = generation_timestamp_value;
	if (timestamp_index != 0u)
	{
		uint previous_timestamp_index = timestamp_index - 1u;
		uint previous_physical_metadata_index = previous_timestamp_index % full_timestamp_metadata_capacity;
		
		_Timestamp_Metadata previous_metadata = timestamp_metadata.metadata[previous_physical_metadata_index];
		if (previous_metadata.generation == metadata.generation)
		{
			uint previous_timestamp_physical_index = previous_timestamp_index % timestamp_capacity;
			previous_timestamp_value = timestamps.timestamps[previous_timestamp_physical_index];
		}
	}

	vec2 bar_vertex = bar_vertices[local_index];

	//float bar_height = 0.00833333333 * 0.1 * float(metadata.generation); //0.5;
	float timestamp_difference = float(timestamp_value - previous_timestamp_value);
	float bar_height = timestamp_difference * nanosecond_height;
	float bar_base = chart_base + float(previous_timestamp_value - generation_timestamp_value) * nanosecond_height;

	gl_Position = vec4(bar_vertex.x * bar_half_width, bar_base + bar_vertex.y * bar_height, 0.0, 1.0);

	timestamp_color = metadata.color;

	gl_CullDistance[0u] = 1.0;
}
