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

const uint generation_in_length_units_uint = 1000u;
const float generation_in_length_units = float(generation_in_length_units_uint);
const float length_unit_in_generations = 1.0 / generation_in_length_units;
const float millisecond_in_nanoseconds = 1000000.0;
const float nanosecond_in_milliseconds = 1.0 / millisecond_in_nanoseconds;

const float step_wise_bar_half_width = 0.01;
const float smooth_bar_half_width = 0.001;

void main()
{
	const float chart_base = -1.0;
	const float bar_half_width = smooth_bar_half_width;
	const float bar_width = 2.0 * bar_half_width;
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

	float normalized_device_coordinate_in_generation = 1.0 / bar_width;
	float camera_generation_steps_from_last_visible_generation = 0.5 - normalized_device_coordinate_in_generation;
	int camera_steps_from_last_visible_generation = int(camera_generation_steps_from_last_visible_generation * generation_in_length_units);
	uint last_visible_generation = next_generation - 1u;
	uint last_visible_generation_x = last_visible_generation * generation_in_length_units_uint;
	float camera_generation_offset_from_last_visible_generation = 0.0;
	int camera_offset_from_last_visible_generation = int(camera_generation_offset_from_last_visible_generation * generation_in_length_units);
	uint camera_x = uint(last_visible_generation_x + camera_offset_from_last_visible_generation + camera_steps_from_last_visible_generation);

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

	float timestamp_difference = float(timestamp_value - previous_timestamp_value);
	float bar_height = timestamp_difference * nanosecond_height;
	float bar_base = chart_base + float(previous_timestamp_value - generation_timestamp_value) * nanosecond_height;

	uint bar_x = metadata.generation * generation_in_length_units_uint;
	float camera_relative_generation = float(int(bar_x - camera_x)) * length_unit_in_generations;
	float camera_relative_bar_x = camera_relative_generation * bar_width;
	float vertex_x_offset = bar_vertex.x * bar_half_width;
	float vertex_x = camera_relative_bar_x + vertex_x_offset;
	gl_Position = vec4(vertex_x, bar_base + bar_vertex.y * bar_height, 0.0, 1.0);

	timestamp_color = metadata.color;

	gl_CullDistance[0u] = 1.0;
}
