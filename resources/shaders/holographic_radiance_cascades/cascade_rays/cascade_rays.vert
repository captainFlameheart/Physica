/* Expected to be concatenated from the CPU:

#define SHOWCASE_CASCADE ?
#define SHOWCASE_SINGLE_RAY ?
#define SHOWCASE_MERGE_TO ?

#define MODE ?;

*/

uniform uvec2 probe_grid_size;
uniform uint cascade;
#if MODE == SHOWCASE_SINGLE_RAY
	uniform uvec2 showcased_ray_texel_position;
#endif

#if MODE == SHOWCASE_CASCADE
	const vec4 cascade_colors[] = 
	{
		vec4(0xFF, 0x00, 0xFF, 1.0),
		vec4(0xFF, 0x00, 0x00, 1.0),
		vec4(0x00, 0xFF, 0x00, 1.0),
		vec4(0x00, 0x00, 0xFF, 1.0),
	};
#endif

uvec2 convert_ray_logical_to_texel_position
(
	in uint rays_per_probe, in uint skipped_rays_below_column, 
	in uint probe_column, in uint probe_row, in uint direction_id
)
{
	return uvec2(probe_column, probe_row * rays_per_probe + direction_id - skipped_rays_below_column);
}

out vec4 line_color;

void main()
{
	uint cascade_power_of_two = 1u << cascade;
	uint lines_per_probe = cascade_power_of_two + 1u;
	uint vertices_per_probe = lines_per_probe << 1u;
	uint vertices_per_probe_column = probe_grid_size.y * vertices_per_probe;
	uint probe_column = gl_VertexID / vertices_per_probe_column;
	uint id_in_probe_column = gl_VertexID - probe_column * vertices_per_probe_column;

	uint probe_x = (1u + probe_column) * cascade_power_of_two;
	uint probe_y = id_in_probe_column / vertices_per_probe;

	uint id_in_probe = id_in_probe_column - probe_y * vertices_per_probe;
	uint direction_index = id_in_probe >> 1u;
	
	#define SHORTEN_DIRECTION 1
	#if SHORTEN_DIRECTION == 1
		vec2 direction = vec2(float(cascade_power_of_two), float(int(direction_index << 1u) - int(cascade_power_of_two)));
		const float removed_length = 0.2;
		float direction_length = length(direction);
		vec2 normalized_direction = direction / direction_length;
		vec2 position = vec2(probe_x, probe_y) + float(gl_VertexID & 1u) * (normalized_direction * (direction_length - removed_length));
		vec2 normalized_probe_distance = 2.0 / vec2(probe_grid_size - 1u);
		vec2 normalized_position = position * normalized_probe_distance - 1.0;

		gl_Position = vec4
		(
			normalized_position,
			0.0, 1.0
		);
	#else
		ivec2 direction = ivec2(cascade_power_of_two, int(direction_index << 1u) - int(cascade_power_of_two));
		ivec2 position = ivec2(probe_x, probe_y) + int(gl_VertexID & 1u) * direction;

		gl_Position = vec4
		(
			float(position.x << 1u) / float(probe_grid_size.x - 1u) - 1.0, 
			float(position.y << 1u) / float(probe_grid_size.y - 1u) - 1.0, 
			0.0, 1.0
		);
	#endif

	#if MODE == SHOWCASE_CASCADE
		float color_factor = float(probe_y & 1u);
		line_color = (1.0 - 2.0 * color_factor) * cascade_colors[cascade % cascade_colors.length()] + color_factor;
	#elif MODE == SHOWCASE_SINGLE_RAY
		uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
		uvec2 ray_texel_position = convert_ray_logical_to_texel_position
		(
			lines_per_probe, skipped_rays_below_column,
			probe_column, probe_y, direction_index
		);
		const float brightness = 0.2 + float(direction_index & 1u) * 0.8;
		line_color = mix(
			vec4(0.0, brightness, 0.0, 0.2),
			vec4(0.0, 1.0, 0.0, 1.0),
			float(ray_texel_position == showcased_ray_texel_position)
		);
	#endif
}
