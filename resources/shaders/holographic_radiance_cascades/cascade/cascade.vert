uniform uvec2 probe_grid_size;
uniform uint cascade;

const vec4 cascade_colors[] = 
{
	vec4(0xFF, 0x00, 0xFF, 1.0),
	vec4(0xFF, 0x00, 0x00, 1.0),
	vec4(0x00, 0xFF, 0x00, 1.0),
	vec4(0x00, 0x00, 0xFF, 1.0),
};

out vec4 line_color;

void main()
{
	uint cascade_power_of_two = 1u << cascade;
	uint lines_per_probe = cascade_power_of_two + 1u;
	uint vertices_per_probe = lines_per_probe << 1u;
	uint vertices_per_probe_column = probe_grid_size.y * vertices_per_probe;
	uint probe_column = gl_VertexID / vertices_per_probe_column;
	uint id_in_probe_column = gl_VertexID % vertices_per_probe_column;

	uint probe_x = probe_column * cascade_power_of_two;
	uint probe_y = id_in_probe_column / vertices_per_probe;

	uint id_in_probe = gl_VertexID % vertices_per_probe;
	uint direction_index = id_in_probe >> 1u;
	
	#define SHORTEN_DIRECTION 1
	#if SHORTEN_DIRECTION == 1
		vec2 direction = vec2(float(cascade_power_of_two), float(int(direction_index << 1u) - int(cascade_power_of_two)));
		const float removed_length = 0.2;
		float direction_length = length(direction);
		vec2 normalized_direction = direction / direction_length;
		vec2 position = vec2(float(probe_x), float(probe_y)) + float(gl_VertexID & 1u) * (normalized_direction * (direction_length - removed_length));

		gl_Position = vec4
		(
			(1.0 + (position.x * 2.0)) / float(probe_grid_size.x) - 1.0, 
			(1.0 + (position.y * 2.0)) / float(probe_grid_size.y) - 1.0, 
			0.0, 1.0
		);
	#else
		ivec2 direction = ivec2(cascade_power_of_two, int(direction_index << 1u) - int(cascade_power_of_two));
		ivec2 position = ivec2(probe_x, probe_y) + int(gl_VertexID & 1u) * direction;

		gl_Position = vec4
		(
			float(1 + (position.x << 1u)) / float(probe_grid_size.x) - 1.0, 
			float(1 + (position.y << 1u)) / float(probe_grid_size.y) - 1.0, 
			0.0, 1.0
		);
	#endif

	float color_factor = float(probe_y & 1u);
	line_color = (1.0 - 2.0 * color_factor) * cascade_colors[cascade % cascade_colors.length()] + color_factor;

	
}
