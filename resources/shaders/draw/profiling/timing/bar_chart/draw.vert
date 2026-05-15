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

void main()
{
	const float bar_half_width = 0.01;

	uint timestamp_index = gl_VertexID / vertices_per_timestamp;
	uint local_index = gl_VertexID - timestamp_index * vertices_per_timestamp;

	uint timestamp_capacity = timing_configuration.timestamp_capacity;
	uint metadata_stage_capacity = timing_configuration.metadata_stage_capacity;
	uint full_timestamp_metadata_capacity = timestamp_capacity + metadata_stage_capacity;
	uint generation_capacity = timing_configuration.generation_capacity;

	uint next_generation = timing_metadata.generation;
	uint next_timestamp = timing_metadata.next_timestamp;

	uint oldest_generation = next_generation - generation_capacity;
	uint oldest_timestamp = next_timestamp - timestamp_capacity;

	timestamp_index = next_timestamp - 1u - timestamp_index;
	uint physical_timestamp_index = timestamp_index % timestamp_capacity;
	uint physical_metadata_index = timestamp_index % full_timestamp_metadata_capacity;

	uint64_t timestamp_value = timestamps.timestamps[physical_timestamp_index];
	_Timestamp_Metadata metadata = timestamp_metadata.metadata[physical_metadata_index];

	/*if (metadata.generation < oldest_generation)
	{
		gl_CullDistance[0u] = -1.0;
		return;
	}*/

	vec2 bar_vertex = bar_vertices[local_index];

	float bar_height = 0.00833333333 * 0.1 * float(metadata.generation); //0.5;

	gl_Position = vec4(bar_vertex.x * bar_half_width, bar_vertex.y * bar_height - 1.0, 0.0, 1.0);

	timestamp_color = vec4(1.0);

	gl_CullDistance[0u] = 1.0;
}
