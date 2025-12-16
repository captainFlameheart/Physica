/* Expected to be concatenated from the CPU:

const float cone_radius = ?;

#define SHOWCASE_CASCADE ?
#define SHOWCASE_SINGLE_CONE ?
#define SHOWCASE_MERGE_TO ?

#define MODE ?;

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

*/

#define EAST_DIRECTION 0
#define NORTH_DIRECTION 1
#define WEST_DIRECTION 2
#define SOUTH_DIRECTION 3

#define DIRECTION EAST_DIRECTION

uniform uvec2 probe_grid_size;
uniform uvec2 source_size;
uniform vec2 probe_padding_factor;
uniform uint cascade;
#if MODE == SHOWCASE_SINGLE_CONE
	uniform uvec2 showcased_cone_texel_position;
#endif
#if MODE == SHOWCASE_MERGE_TO
	uniform uvec2 merged_to_cone_texel_position;
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

vec4 pick_color_if_merging_to
(
	in uvec2 merged_to_texel_position,
	in uint query_cascade_power_of_two, in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	ivec2 output_texel_position = ivec2(merged_to_texel_position);
	ivec2 query_texel_position = ivec2(convert_cone_logical_to_texel_position(query_cascade_power_of_two, query_probe_column, query_probe_row, query_direction_id));

	int cascade_merged_to = int(cascade) - 1;
	
	int edge_width = int(probe_grid_size.x) - 1;
	int edge_height = int(probe_grid_size.y) - 1;
	
	int edge_width_decremented = edge_width - 1;

	int fluence_gathering_data_direction_mask = (1 << cascade_merged_to) - 1;
	int fluence_gathering_data_cascade = cascade_merged_to;
	int fluence_gathering_data_cascade_power_of_two = 1 << cascade_merged_to;
	int fluence_gathering_data_upper_cascade_probe_column_texel_x_mask = (-1) << int(cascade);
	int fluence_gathering_data_max_fluence_probe_column_texel_x = (edge_width_decremented >> int(cascade)) << int(cascade);
	int fluence_gathering_data_upper_cascade = int(cascade);
	int fluence_gathering_data_max_fluence_probe_y = edge_height;

	// From here we replicate the fluence gather logic.

	int direction_id = output_texel_position.x & fluence_gathering_data_direction_mask;
	int lower_direction_id = direction_id << 1;
	int probe_column = output_texel_position.x >> fluence_gathering_data_cascade;

	float cascade_power_of_two_float = float(fluence_gathering_data_cascade_power_of_two); // TODO: Put in uniform block
	float middle_angle = atan(float((lower_direction_id + 1) - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float);

	float lower_angle = middle_angle - atan(float(lower_direction_id - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float);

	int interpolating = probe_column & 1;

	int lower_y_offset = lower_direction_id - fluence_gathering_data_cascade_power_of_two;	// TODO: This expression is used by lower angle

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);

	// IMPORTANT TODO: The clamping should not be necessary here, but it might be necessary when the fluence is added to the final fluence 
	// buffer.
	int near_sample_probe_column_texel_x = min(
		output_texel_position.x & fluence_gathering_data_upper_cascade_probe_column_texel_x_mask, fluence_gathering_data_max_fluence_probe_column_texel_x
	);
	int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_direction_id;
	int lower_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * lower_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	int lower_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data_upper_cascade), fluence_gathering_data_max_fluence_probe_column_texel_x
	) + lower_direction_id;
	int lower_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * lower_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	float interpolating_float = float(interpolating);

	int upper_direction_id = lower_direction_id + 2;
	int upper_y_offset = upper_direction_id - fluence_gathering_data_cascade_power_of_two; // TODO: This expression is used by upper angle

	float upper_angle = atan(float(upper_direction_id - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float) - middle_angle;

	int upper_near_fluence_sample_x = near_sample_probe_column_texel_x + upper_direction_id - 1;
	int upper_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * upper_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	int upper_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data_upper_cascade), fluence_gathering_data_max_fluence_probe_column_texel_x
	) + upper_direction_id - 1;	// TODO: Many expressions that have been evaluated earlier
	int upper_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * upper_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);
	
	// End of gather replication

	bool query_is_lower_near_fluence = query_texel_position.x == lower_near_fluence_sample_x && query_texel_position.y == lower_near_fluence_sample_y;
	bool query_is_lower_far_fluence = query_texel_position.x == lower_far_fluence_sample_x && query_texel_position.y == lower_far_fluence_sample_y;
	
	bool query_is_upper_near_fluence = query_texel_position.x == upper_near_fluence_sample_x && query_texel_position.y == upper_near_fluence_sample_y;
	bool query_is_upper_far_fluence = query_texel_position.x == upper_far_fluence_sample_x && query_texel_position.y == upper_far_fluence_sample_y;

	const float active_alpha = 1.0;

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	vec4 color = vec4(brightness, brightness, brightness, 0.2);
	if (query_is_lower_near_fluence || query_is_lower_far_fluence || query_is_upper_near_fluence || query_is_upper_far_fluence)
	{
		color = vec4(0.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_near_fluence)
	{
		color += vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_fluence)
	{
		color += vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_near_fluence)
	{
		color += vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_far_fluence)
	{
		color += vec4(0.0, 1.0, 1.0, active_alpha);
	}
	return color;
}

void main()
{
	uvec2 rotated_probe_grid_size;
	#if DIRECTION == EAST_DIRECTION
		rotated_probe_grid_size = probe_grid_size;
	#elif DIRECTION == NORTH_DIRECTION
		rotated_probe_grid_size = uvec2(probe_grid_size.y, probe_grid_size.x);
	#elif DIRECTION == WEST_DIRECTION
		rotated_probe_grid_size = probe_grid_size;
	#elif DIRECTION == SOUTH_DIRECTION
		rotated_probe_grid_size = uvec2(probe_grid_size.y, probe_grid_size.x);
	#endif

	uint cascade_power_of_two = 1u << cascade;
	uint vertices_per_probe = cascade_power_of_two * 3u;
	uint vertices_per_probe_column = rotated_probe_grid_size.y * vertices_per_probe;
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
	vec2 padding = probe_padding_factor / vec2(source_size);
	vec2 normalized_probe_distance = (2.0 + 2.0 * padding) / vec2(rotated_probe_grid_size - 1u);
	vec2 normalized_position = position * normalized_probe_distance - 1.0 - padding;

	gl_Position = vec4
	(
		normalized_position,
		0.0, 1.0
	);

	#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		gl_Position.xy *= 0.5;
	#endif

	#if DIRECTION == EAST_DIRECTION
	#elif DIRECTION == NORTH_DIRECTION
		gl_Position.xy = vec2(-gl_Position.y, gl_Position.x);
	#elif DIRECTION == WEST_DIRECTION
		gl_Position.xy = vec2(-gl_Position.x, -gl_Position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		gl_Position.xy = vec2(gl_Position.y, -gl_Position.x);
	#endif

	#if MODE == SHOWCASE_CASCADE
		float color_factor = float(lower_direction_index & 1u);
		cone_color = cascade_colors[cascade % cascade_colors.length()];
		cone_color = vec4((1.0 - 2.0 * color_factor) * cone_color.rgb + color_factor, cone_color.a);
	#elif MODE == SHOWCASE_SINGLE_CONE
		uvec2 cone_texel_position = convert_cone_logical_to_texel_position(cascade_power_of_two, probe_column, probe_y, lower_direction_index);
		const float brightness = 0.2 + float(lower_direction_index & 1u) * 0.8;
		cone_color = mix(
			vec4(0.0, brightness, 0.0, 0.2),
			vec4(0.0, 1.0, 0.0, 1.0),
			float(cone_texel_position == showcased_cone_texel_position)
		);
	#elif MODE == SHOWCASE_MERGE_TO
		cone_color = pick_color_if_merging_to
		(
			merged_to_cone_texel_position, 
			cascade_power_of_two, probe_column, probe_y, lower_direction_index
		);
	#endif
}
