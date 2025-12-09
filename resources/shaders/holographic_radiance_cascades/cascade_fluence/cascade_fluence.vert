/* Expected to be concatenated from the CPU:

const float cone_radius = ?;

#define SHOWCASE_CASCADE ?
#define SHOWCASE_SINGLE_CONE ?

#define MODE ?;

*/

uniform uvec2 probe_grid_size;
uniform uint cascade;
#if MODE == SHOWCASE_SINGLE_CONE
	uniform uvec2 showcased_cone_texel_position;
#endif

#if MODE == SHOWCASE_CASCADE
	const vec4 cascade_colors[] = 
	{
		vec4(0x00, 0x00, 0xFF, 0.2),
		vec4(0x00, 0xFF, 0x00, 0.2),
		vec4(0xFF, 0x00, 0x00, 0.2),
		vec4(0xFF, 0x00, 0xFF, 0.2),
	};
#endif

out vec2 offset;
out vec4 cone_color;

uvec2 convert_cone_logical_to_texel_position(in uint cascade_power_of_two, in uint probe_column, in uint probe_row, in uint direction_id)
{
	return uvec2(probe_column * cascade_power_of_two + direction_id, probe_row);
}

void main()
{
	uint cascade_power_of_two = 1u << cascade;
	uint vertices_per_probe = cascade_power_of_two * 3u;
	uint vertices_per_probe_column = probe_grid_size.y * vertices_per_probe;
	uint probe_column = gl_VertexID / vertices_per_probe_column;
	uint id_in_probe_column = gl_VertexID - probe_column * vertices_per_probe_column;

	uint probe_x = (1u + probe_column) * cascade_power_of_two;
	uint probe_y = id_in_probe_column / vertices_per_probe;

	uint id_in_probe = id_in_probe_column - probe_y * vertices_per_probe;
	uint lower_direction_index = id_in_probe / 3u;
	uint id_in_cone = id_in_probe - lower_direction_index * 3u;

	float cascade_power_of_two_float = float(cascade_power_of_two);
	int lower_direction_y = int(lower_direction_index << 1u) - int(cascade_power_of_two);
	float lower_direction_y_float = float(lower_direction_y);
	float upper_direction_y_float = float(lower_direction_y + 2);

	float cone_radius_factor = cone_radius / cascade_power_of_two_float;
	float at_lower_end = float(id_in_cone & 1u);
	float at_upper_end = float(id_in_cone >> 1u);
	float at_end = at_lower_end + at_upper_end;
	offset = cone_radius_factor * vec2
	(
		at_end * cascade_power_of_two_float, 
		at_lower_end * lower_direction_y_float + at_upper_end * upper_direction_y_float
	);
	vec2 position = vec2(probe_x, probe_y) + offset;
	vec2 normalized_probe_distance = 2.0 / vec2(probe_grid_size - 1u);
	vec2 normalized_position = position * normalized_probe_distance - 1.0;

	gl_Position = vec4
	(
		normalized_position,
		0.0, 1.0
	);

	#if MODE == SHOWCASE_CASCADE
		float color_factor = float(lower_direction_index & 1u);
		cone_color = cascade_colors[cascade % cascade_colors.length()];
		cone_color = vec4((1.0 - 2.0 * color_factor) * cone_color.rgb + color_factor, cone_color.a);
	#elif MODE == SHOWCASE_SINGLE_CONE
		uvec2 cone_texel_position = convert_cone_logical_to_texel_position(cascade_power_of_two, probe_column, probe_y, lower_direction_index);
		const float brightness = 1.0;
		cone_color = mix(
			vec4(brightness, brightness, brightness, 0.1),
			vec4(0.0, 1.0, 0.0, 1.0),
			float(cone_texel_position == showcased_cone_texel_position)
		);
	#endif
}
