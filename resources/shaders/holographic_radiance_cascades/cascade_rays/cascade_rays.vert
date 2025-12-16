/* Expected to be concatenated from the CPU:

#define SHOWCASE_CASCADE ?
#define SHOWCASE_SINGLE_RAY ?
#define SHOWCASE_MERGE_TO_RAY ?
#define SHOWCASE_MERGE_TO_CONE ?
#define SHOWCASE_RADIANCE ?

#define MODE ?;

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

#define COLUMN_RAY_TEXTURE_MODE ?
#define ROW_RAY_TEXTURE_MODE ?

#define RAY_TEXTURE_MODE ?

*/

#define EAST_DIRECTION 0
#define NORTH_DIRECTION 1
#define WEST_DIRECTION 2
#define SOUTH_DIRECTION 3

#define DIRECTION NORTH_DIRECTION

uniform uvec2 probe_grid_size;
uniform uvec2 source_size;
uniform vec2 probe_padding_factor;
uniform uint cascade;
#if MODE == SHOWCASE_SINGLE_RAY
	uniform uvec2 showcased_ray_texel_position;
#elif MODE == SHOWCASE_MERGE_TO_RAY
	uniform uvec2 merged_to_ray_texel_position;
#elif MODE == SHOWCASE_MERGE_TO_CONE
	uniform uvec2 merged_to_cone_texel_position;
#elif MODE == SHOWCASE_RADIANCE
	uniform sampler2DArray rays;
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

uvec2 convert_ray_logical_to_texel_position_in_row_ray_mode
(
	in uint rays_per_probe,
	in uint probe_column, in uint probe_row, in uint direction_id
)
{
	uvec2 texel_position = uvec2(probe_column * rays_per_probe + direction_id, probe_row);
	#if DIRECTION == EAST_DIRECTION
	#elif DIRECTION == NORTH_DIRECTION
		texel_position = uvec2((probe_grid_size.x - 1u) - texel_position.y, texel_position.x);
	#elif DIRECTION == WEST_DIRECTION
		texel_position = uvec2((probe_grid_size.x - 1u) - texel_position.x, (probe_grid_size.y - 1u) - texel_position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		texel_position = uvec2(texel_position.y, (probe_grid_size.y - 1u) - texel_position.x);
	#endif
	return texel_position;
}

uvec2 convert_ray_logical_to_texel_position
(
	in uint rays_per_probe, in uint skipped_rays_below_column, 
	in uint probe_column, in uint probe_row, in uint direction_id
)
{
	return uvec2(probe_column, probe_row * rays_per_probe + direction_id - skipped_rays_below_column);
}

vec4 pick_color_if_merging_to_ray_in_row_ray_mode
(
	in uvec2 merged_to_texel_position,
	in uint query_rays_per_probe,
	in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	// IMPORTANT: ray_casting_data_lower_cascade_max_ray_probe_row should be renamed!

	ivec2 query_texel_position = ivec2(convert_ray_logical_to_texel_position_in_row_ray_mode(
		query_rays_per_probe, 
		query_probe_column, query_probe_row, query_direction_id
	));

	int cascade_merging_from = int(cascade);
	int cascade_merging_from_power_of_two = 1 << cascade_merging_from;
	int merged_to_cascade = cascade_merging_from + 1;
	int merged_to_cascade_power_of_two = 1 << merged_to_cascade;

	int edge_width = int(probe_grid_size.x) - 1;
	int edge_width_decremented = edge_width - 1;
	int edge_height = int(probe_grid_size.y) - 1;

	int ray_casting_data_rays_per_probe = merged_to_cascade_power_of_two + 1;
	//int ray_casting_data_skipped_rays_below_column = (ray_casting_data_rays_per_probe + 1) >> 1;
	int ray_casting_data_lower_cascade_rays_per_probe = cascade_merging_from_power_of_two + 1;
	//int ray_casting_data_lower_cascade_skipped_rays_below_column = (ray_casting_data_lower_cascade_rays_per_probe + 1) >> 1;
	//int ray_casting_data_lower_cascade_max_ray_probe_column = (edge_width - 1) / cascade_merging_from_power_of_two - 1;
	//int ray_casting_data_lower_cascade_max_ray_probe_row = int(probe_grid_size.y) * ray_casting_data_lower_cascade_rays_per_probe - (ray_casting_data_lower_cascade_skipped_rays_below_column << 1) - 1;
	//int ray_casting_data_g = ray_casting_data_lower_cascade_rays_per_probe << 1; //ray_casting_data_rays_per_probe << 1;
	//int ray_casting_data_f = ray_casting_data_lower_cascade_rays_per_probe << cascade_merging_from; //ray_casting_data_rays_per_probe << merged_to_cascade;

	int lower_cascade_min_outside_probe_x = (edge_width_decremented + cascade_merging_from_power_of_two) >> cascade_merging_from;
	int ray_casting_data_lower_cascade_max_probe_column_texel_x = (lower_cascade_min_outside_probe_x - 2) * ray_casting_data_lower_cascade_rays_per_probe;
	int ray_casting_data_lower_cascade_power_of_two = cascade_merging_from_power_of_two;
	int ray_casting_data_lower_cascade_max_probe_row = edge_height;

	ivec2 output_texel_position = ivec2(merged_to_texel_position);

	// From here we replicate the ray merge logic.

	//ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int probe_column = output_texel_position.x / ray_casting_data_rays_per_probe;
	int probe_column_texel_x = probe_column * ray_casting_data_rays_per_probe;
	int direction_id = output_texel_position.x - probe_column_texel_x;

	int lower_cascade_near_probe_column_texel_x = 
		((probe_column << 1) | 1) * ray_casting_data_lower_cascade_rays_per_probe;
	
	int lower_direction_id = direction_id >> 1;

	// Lower near
	int lower_near_texel_x = lower_cascade_near_probe_column_texel_x + lower_direction_id;	// Does not need clamping.
	//radiance = texelFetch(shorter_rays, ivec3(lower_near_texel_x, output_texel_position.y, 0), 0);
	//transmittance = texelFetch(shorter_rays, ivec3(lower_near_texel_x, output_texel_position.y, 1), 0);
	
	int upper_direction_id = (direction_id + 1) >> 1;

	// Upper near
	int upper_near_texel_x = lower_cascade_near_probe_column_texel_x + upper_direction_id;	// Does not need clamping.
	//radiance += texelFetch(shorter_rays, ivec3(upper_near_texel_x, output_texel_position.y, 0), 0);
	//vec4 upper_transmittance = texelFetch(shorter_rays, ivec3(upper_near_texel_x, output_texel_position.y, 1), 0);

	int lower_cascade_far_probe_column_texel_x = 
		lower_cascade_near_probe_column_texel_x + ray_casting_data_lower_cascade_rays_per_probe;
	int clamped_lower_cascade_far_probe_column_texel_x = 
		min(lower_cascade_far_probe_column_texel_x, ray_casting_data_lower_cascade_max_probe_column_texel_x);
	float lower_cascade_far_probe_column_is_inside_bounds = float
	(
		lower_cascade_far_probe_column_texel_x <= ray_casting_data_lower_cascade_max_probe_column_texel_x
	);
	int far_base_y = output_texel_position.y - ray_casting_data_lower_cascade_power_of_two;

	// Lower far
	int clamped_lower_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + upper_direction_id;
	int lower_far_texel_y = far_base_y + (lower_direction_id << 1);
	int clamped_lower_far_texel_y = clamp(lower_far_texel_y, 0, ray_casting_data_lower_cascade_max_probe_row);
	//radiance += transmittance * 
	//	(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, ivec3(clamped_lower_far_texel_x, clamped_lower_far_texel_y, 0), 0));
	//transmittance *= mix
	//(
	//	vec4(1.0),
	//	texelFetch(shorter_rays, ivec3(clamped_lower_far_texel_x, clamped_lower_far_texel_y, 1), 0),
	//	lower_cascade_far_probe_column_is_inside_bounds
	//);

	// Upper far
	int clamped_upper_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + lower_direction_id;
	int upper_far_texel_y = far_base_y + (upper_direction_id << 1);
	int clamped_upper_far_texel_y = clamp(upper_far_texel_y, 0, ray_casting_data_lower_cascade_max_probe_row);
	//radiance += upper_transmittance * 
	//	(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, ivec3(clamped_upper_far_texel_x, clamped_upper_far_texel_y, 0), 0));
	//upper_transmittance *= mix
	//(
	//	vec4(1.0),
	//	texelFetch(shorter_rays, ivec3(clamped_upper_far_texel_x, clamped_upper_far_texel_y, 1), 0),
	//	lower_cascade_far_probe_column_is_inside_bounds
	//);

	//transmittance += upper_transmittance;

	//radiance *= 0.5;
	//transmittance *= 0.5;

	// End of merge replication.

	int final_lower_near_texel_x = lower_near_texel_x;
	int final_lower_near_texel_y = output_texel_position.y;
	
	int final_lower_far_texel_x = clamped_lower_far_texel_x;
	int final_lower_far_texel_y = clamped_lower_far_texel_y;

	int final_upper_near_texel_x = upper_near_texel_x;
	int final_upper_near_texel_y = output_texel_position.y;

	int final_upper_far_texel_x = clamped_upper_far_texel_x;
	int final_upper_far_texel_y = clamped_upper_far_texel_y;

	bool query_is_lower_near_ray = query_texel_position.x == final_lower_near_texel_x && query_texel_position.y == final_lower_near_texel_y;
	bool query_is_lower_far_ray = query_texel_position.x == final_lower_far_texel_x && query_texel_position.y == final_lower_far_texel_y;

	bool query_is_upper_near_ray = query_texel_position.x == final_upper_near_texel_x && query_texel_position.y == final_upper_near_texel_y;
	bool query_is_upper_far_ray = query_texel_position.x == final_upper_far_texel_x && query_texel_position.y == final_upper_far_texel_y;

	const float active_alpha = 1.0;

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	vec4 color = vec4(brightness, brightness, brightness, 0.2);
	if (query_is_lower_near_ray || query_is_lower_far_ray || query_is_upper_near_ray || query_is_upper_far_ray)
	{
		color = vec4(0.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_near_ray)
	{
		color += vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_ray)
	{
		color += vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_near_ray)
	{
		color += vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_far_ray)
	{
		color += vec4(0.0, 1.0, 1.0, active_alpha);
	}
	return color;
}

vec4 pick_color_if_merging_to_ray
(
	in uvec2 merged_to_texel_position,
	in uint query_rays_per_probe, in uint query_skipped_rays_below_column,
	in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	ivec2 query_texel_position = ivec2(convert_ray_logical_to_texel_position(
		query_rays_per_probe, query_skipped_rays_below_column,
		query_probe_column, query_probe_row, query_direction_id
	));
	
	int cascade_merging_from = int(cascade);
	int cascade_merging_from_power_of_two = 1 << cascade_merging_from;
	int merged_to_cascade = cascade_merging_from + 1;
	int merged_to_cascade_power_of_two = 1 << merged_to_cascade;

	int edge_width = int(probe_grid_size.x) - 1;
	int edge_height = int(probe_grid_size.y) - 1;

	int ray_casting_data_rays_per_probe = merged_to_cascade_power_of_two + 1;
	int ray_casting_data_skipped_rays_below_column = (ray_casting_data_rays_per_probe + 1) >> 1;
	int ray_casting_data_lower_cascade_rays_per_probe = cascade_merging_from_power_of_two + 1;
	int ray_casting_data_lower_cascade_skipped_rays_below_column = (ray_casting_data_lower_cascade_rays_per_probe + 1) >> 1;
	int ray_casting_data_lower_cascade_max_ray_probe_column = (edge_width - 1) / cascade_merging_from_power_of_two - 1;
	int ray_casting_data_lower_cascade_max_ray_probe_row = int(probe_grid_size.y) * ray_casting_data_lower_cascade_rays_per_probe - (ray_casting_data_lower_cascade_skipped_rays_below_column << 1) - 1;
	int ray_casting_data_g = ray_casting_data_lower_cascade_rays_per_probe << 1; //ray_casting_data_rays_per_probe << 1;
	int ray_casting_data_f = ray_casting_data_lower_cascade_rays_per_probe << cascade_merging_from; //ray_casting_data_rays_per_probe << merged_to_cascade;

	ivec2 output_texel_position = ivec2(merged_to_texel_position);

	// From here we replicate the ray merge logic.

	int ray_id_in_column = ray_casting_data_skipped_rays_below_column + output_texel_position.y;
	int probe_y = ray_id_in_column / ray_casting_data_rays_per_probe;
	int direction_id = ray_id_in_column - probe_y * ray_casting_data_rays_per_probe;

	// Some temporaries
	int a = (output_texel_position.x << 1) | 1;	// Does not need clamping
	int b = ray_casting_data_lower_cascade_rays_per_probe * probe_y - ray_casting_data_lower_cascade_skipped_rays_below_column;
	int d = direction_id >> 1;
	int h = b + d;

	int clamped_h = max(0, h);
	float h_is_inside = float(0 <= h);

	// And some more temporaries needed from here
	int e = (direction_id + 1) >> 1;
	int i = b + e;
	int c = a + 1;

	int clamped_c = min(c, ray_casting_data_lower_cascade_max_ray_probe_column);
	float c_is_inside = float(c <= ray_casting_data_lower_cascade_max_ray_probe_column);

	// Lower far
	int lower_far_sample_y = i + d * ray_casting_data_g - ray_casting_data_f;
	int clamped_lower_far_sample_y = clamp(lower_far_sample_y, 0, ray_casting_data_lower_cascade_max_ray_probe_row);
	float lower_far_sample_y_is_inside = float(0 <= lower_far_sample_y) * float(lower_far_sample_y <= ray_casting_data_lower_cascade_max_ray_probe_row);

	int clamped_i = min(i, ray_casting_data_lower_cascade_max_ray_probe_row);
	float i_is_inside = float(i <= ray_casting_data_lower_cascade_max_ray_probe_row);

	// Upper far
	int upper_far_sample_y = h + e * ray_casting_data_g - ray_casting_data_f;	// Does not need to be clamped
	int clamped_upper_far_sample_y = clamp(upper_far_sample_y, 0, ray_casting_data_lower_cascade_max_ray_probe_row);
	float upper_far_sample_y_is_inside = float(0 <= upper_far_sample_y) * float(upper_far_sample_y <= ray_casting_data_lower_cascade_max_ray_probe_row);

	// End of merge replication.

	int lower_near_texel_x = a;
	int lower_near_texel_y = clamped_h;
	
	int lower_far_texel_x = clamped_c;
	int lower_far_texel_y = clamped_lower_far_sample_y;

	int upper_near_texel_x = a;
	int upper_near_texel_y = clamped_i;

	int upper_far_texel_x = clamped_c;
	int upper_far_texel_y = clamped_upper_far_sample_y;

	bool query_is_lower_near_ray = query_texel_position.x == lower_near_texel_x && query_texel_position.y == lower_near_texel_y;
	bool query_is_lower_far_ray = query_texel_position.x == lower_far_texel_x && query_texel_position.y == lower_far_texel_y;

	bool query_is_upper_near_ray = query_texel_position.x == upper_near_texel_x && query_texel_position.y == upper_near_texel_y;
	bool query_is_upper_far_ray = query_texel_position.x == upper_far_texel_x && query_texel_position.y == upper_far_texel_y;

	const float active_alpha = 1.0;

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	vec4 color = vec4(brightness, brightness, brightness, 0.2);
	if (query_is_lower_near_ray || query_is_lower_far_ray || query_is_upper_near_ray || query_is_upper_far_ray)
	{
		color = vec4(0.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_near_ray)
	{
		color += vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_ray)
	{
		color += vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_near_ray)
	{
		color += vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_far_ray)
	{
		color += vec4(0.0, 1.0, 1.0, active_alpha);
	}
	return color;
}

vec4 pick_color_if_merging_to_cone_in_row_ray_mode
(
	in uvec2 merged_to_texel_position,
	in uint query_rays_per_probe,
	in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	ivec2 query_texel_position = ivec2(convert_ray_logical_to_texel_position_in_row_ray_mode(
		query_rays_per_probe, 
		query_probe_column, query_probe_row, query_direction_id
	));

	int cascade_merged_to = int(cascade);
	int upper_cascade = cascade_merged_to + 1;
	
	int edge_width = int(probe_grid_size.x) - 1;
	int edge_height = int(probe_grid_size.y) - 1;
	
	int edge_width_decremented = edge_width - 1;

	int fluence_gathering_data_direction_mask = (1 << cascade_merged_to) - 1;
	int fluence_gathering_data_cascade = cascade_merged_to;
	int fluence_gathering_data_cascade_power_of_two = 1 << cascade_merged_to;
	int fluence_gathering_data_upper_cascade_probe_column_texel_x_mask = (-1) << upper_cascade;
	int fluence_gathering_data_max_fluence_probe_column_texel_x = (edge_width_decremented >> upper_cascade) << upper_cascade;
	int fluence_gathering_data_upper_cascade = upper_cascade;
	int fluence_gathering_data_max_fluence_probe_y = edge_height;

	int fluence_gathering_data_max_ray_probe_column = (edge_width - 1) / fluence_gathering_data_cascade_power_of_two - 1;
	int fluence_gathering_data_rays_per_probe = fluence_gathering_data_cascade_power_of_two + 1;
	int fluence_gathering_data_skipped_rays_below_column = (fluence_gathering_data_rays_per_probe + 1) >> 1;
	int fluence_gathering_data_max_ray_probe_row = int(probe_grid_size.y) * fluence_gathering_data_rays_per_probe - (fluence_gathering_data_skipped_rays_below_column << 1) - 1;

	ivec2 output_texel_position = ivec2(merged_to_texel_position);

	vec4 fluence;

	// From here we replicate the fluence gather logic.

	// IMPORTANT TODO: REORDER TEXTURE FETCHES BASED ON CHACHE LOCALITY
	//ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int direction_id = output_texel_position.x & fluence_gathering_data_direction_mask;
	int lower_direction_id = direction_id << 1;
	int probe_column = output_texel_position.x >> fluence_gathering_data_cascade;
	
	int clamped_near_ray_column_texel_x = min(probe_column, fluence_gathering_data_max_ray_probe_column) * fluence_gathering_data_rays_per_probe;
	float near_ray_is_inside = float(probe_column <= fluence_gathering_data_max_ray_probe_column);

	float cascade_power_of_two_float = float(fluence_gathering_data_cascade_power_of_two); // TODO: Put in uniform block
	float middle_angle = atan(float((lower_direction_id + 1) - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float);

	float lower_angle = middle_angle - atan(float(lower_direction_id - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float);

	// Lower near ray
	int clamped_lower_near_ray_texel_x = clamped_near_ray_column_texel_x + direction_id;
	//fluence = (lower_angle * near_ray_is_inside) * texelFetch(rays, ivec3(clamped_lower_near_ray_texel_x, output_texel_position.y, 0), 0);
	//vec4 lower_near_transmittance = mix
	//(
	//	vec4(1.0), texelFetch(rays, ivec3(clamped_lower_near_ray_texel_x, output_texel_position.y, 1), 0,
	//	near_ray_is_inside
	//);
	
	//vec4 shifted_lower_near_transmittance = lower_near_transmittance - 1.0;

	int interpolating = probe_column & 1;

	int lower_y_offset = lower_direction_id - fluence_gathering_data_cascade_power_of_two;	// TODO: This expression is used by lower angle

	int far_ray_column = probe_column + interpolating;
	int clamped_far_ray_column_texel_x = min(far_ray_column, fluence_gathering_data_max_ray_probe_column) * fluence_gathering_data_rays_per_probe;
	float far_ray_x_is_inside = float(far_ray_column <= fluence_gathering_data_max_ray_probe_column);

	// Lower far ray
	int clamped_lower_far_ray_texel_x = clamped_far_ray_column_texel_x + direction_id;
	int clamped_lower_far_ray_texel_y = clamp(output_texel_position.y + interpolating * lower_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);
	//vec4 lower_far_fluence = (lower_angle * far_ray_x_is_inside) * texelFetch(rays, ivec3(clamped_lower_far_ray_texel_x, clamped_lower_far_ray_texel_y, 0), 0);
	//vec4 lower_far_transmittance = mix(
	//	vec4(1.0), texelFetch(rays, ivec3(clamped_lower_far_ray_texel_x, clamped_lower_far_ray_texel_y, 1), 0), 
	//	far_ray_x_is_inside
	//);

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);
	//vec4 lower_near_transmit_factor = 1.0 + not_interpolating_float * shifted_lower_near_transmittance;

	// IMPORTANT TODO: The clamping should not be necessary here, but it might be necessary when the fluence is added to the final fluence 
	// buffer.
	int near_sample_probe_column_texel_x = min(
		output_texel_position.x & fluence_gathering_data_upper_cascade_probe_column_texel_x_mask, fluence_gathering_data_max_fluence_probe_column_texel_x
	);
	int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_direction_id;
	int lower_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * lower_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	// Lower near fluence
	//fluence += lower_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(lower_near_fluence_sample_x, lower_near_fluence_sample_y, fluence_gathering_data_upper_cascade_fluence_layer), 0);

	int lower_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data_upper_cascade), fluence_gathering_data_max_fluence_probe_column_texel_x
	) + lower_direction_id;
	int lower_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * lower_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	// Lower far fluence
	//lower_far_fluence += lower_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(lower_far_fluence_sample_x, lower_far_fluence_sample_y, fluence_gathering_data_upper_cascade_fluence_layer), 0);

	float interpolating_float = float(interpolating);
	//vec4 lower_far_transmit_factor = 1.0 + interpolating_float * shifted_lower_near_transmittance;
	//fluence += lower_far_transmit_factor * lower_far_fluence;

	//int upper_near_ray_sample_y = lower_near_ray_sample_y + 1;

	int upper_direction_id = lower_direction_id + 2;
	int upper_y_offset = upper_direction_id - fluence_gathering_data_cascade_power_of_two; // TODO: This expression is used by upper angle

	float upper_angle = atan(float(upper_direction_id - fluence_gathering_data_cascade_power_of_two) / cascade_power_of_two_float) - middle_angle;

	// Upper near ray
	int clamped_upper_near_ray_texel_x = clamped_lower_near_ray_texel_x + 1;
	//fluence += (upper_angle * near_ray_is_inside) * texelFetch(rays, ivec3(clamped_upper_near_ray_texel_x, output_texel_position.y, 0), 0);
	//vec4 upper_near_transmittance = mix(
	//	vec4(1.0), texelFetch(rays, ivec3(clamped_upper_near_ray_texel_x, output_texel_position.y, 1), 0), 
	//	near_ray_is_inside
	//);

	//vec4 shifted_upper_near_transmittance = upper_near_transmittance - 1.0;

	// Upper far ray
	int clamped_upper_far_ray_texel_x = clamped_lower_far_ray_texel_x + 1;
	int clamped_upper_far_ray_texel_y = clamp(output_texel_position.y + interpolating * upper_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);
	//vec4 upper_far_fluence = (upper_angle * far_ray_x_is_inside) * texelFetch(rays, ivec3(clamped_upper_far_ray_texel_x, clamped_upper_far_ray_texel_y, 0), 0);
	//vec4 upper_far_transmittance = mix(
	//	vec4(1.0), texelFetch(rays, ivec3(clamped_upper_far_ray_texel_x, clamped_upper_far_ray_texel_y, 1), 0), 
	//	far_ray_x_is_inside
	//);

	//vec4 upper_near_transmit_factor = 1.0 + not_interpolating_float * shifted_upper_near_transmittance;

	int upper_near_fluence_sample_x = near_sample_probe_column_texel_x + upper_direction_id - 1;
	int upper_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * upper_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);

	// Upper near fluence
	//fluence += upper_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(upper_near_fluence_sample_x, upper_near_fluence_sample_y, fluence_gathering_data_upper_cascade_fluence_layer), 0);

	int upper_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data_upper_cascade), fluence_gathering_data_max_fluence_probe_column_texel_x
	) + upper_direction_id - 1;	// TODO: Many expressions that have been evaluated earlier
	int upper_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * upper_y_offset, 0, fluence_gathering_data_max_fluence_probe_y);
	
	// Upper far fluence
	//upper_far_fluence += upper_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(upper_far_fluence_sample_x, upper_far_fluence_sample_y, fluence_gathering_data_upper_cascade_fluence_layer), 0);

	//vec4 upper_far_transmit_factor = 1.0 + interpolating_float * shifted_upper_near_transmittance;
	//fluence += upper_far_transmit_factor * upper_far_fluence;

	fluence *= 0.5;

	// End of fluence gather replication.
	
	bool query_is_lower_near_ray = query_texel_position.x == clamped_lower_near_ray_texel_x && query_texel_position.y == output_texel_position.y;
	bool query_is_lower_far_ray = query_texel_position.x == clamped_lower_far_ray_texel_x && query_texel_position.y == clamped_lower_far_ray_texel_y;
	
	bool query_is_upper_near_ray = query_texel_position.x == clamped_upper_near_ray_texel_x && query_texel_position.y == output_texel_position.y;
	bool query_is_upper_far_ray = query_texel_position.x == clamped_upper_far_ray_texel_x && query_texel_position.y == clamped_upper_far_ray_texel_y;

	const float active_alpha = 1.0;

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	vec4 color = vec4(brightness, brightness, brightness, 0.2);
	if (query_is_lower_near_ray || query_is_lower_far_ray || query_is_upper_near_ray || query_is_upper_far_ray)
	{
		color = vec4(0.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_near_ray)
	{
		color += vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_ray)
	{
		color += vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_near_ray)
	{
		color += vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_far_ray)
	{
		color += vec4(0.0, 1.0, 1.0, active_alpha);
	}
	return color;
}

vec4 pick_color_if_merging_to_cone
(
	in uvec2 merged_to_texel_position,
	in uint query_rays_per_probe, in uint query_skipped_rays_below_column,
	in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	ivec2 query_texel_position = ivec2(convert_ray_logical_to_texel_position(
		query_rays_per_probe, query_skipped_rays_below_column,
		query_probe_column, query_probe_row, query_direction_id
	));

	int cascade_merged_to = int(cascade);
	int upper_cascade = cascade_merged_to + 1;
	
	int edge_width = int(probe_grid_size.x) - 1;
	int edge_height = int(probe_grid_size.y) - 1;
	
	int edge_width_decremented = edge_width - 1;

	int fluence_gathering_data_direction_mask = (1 << cascade_merged_to) - 1;
	int fluence_gathering_data_cascade = cascade_merged_to;
	int fluence_gathering_data_cascade_power_of_two = 1 << cascade_merged_to;
	int fluence_gathering_data_upper_cascade_probe_column_texel_x_mask = (-1) << upper_cascade;
	int fluence_gathering_data_max_fluence_probe_column_texel_x = (edge_width_decremented >> upper_cascade) << upper_cascade;
	int fluence_gathering_data_upper_cascade = upper_cascade;
	int fluence_gathering_data_max_fluence_probe_y = edge_height;

	int fluence_gathering_data_max_ray_probe_column = (edge_width - 1) / fluence_gathering_data_cascade_power_of_two - 1;
	int fluence_gathering_data_rays_per_probe = fluence_gathering_data_cascade_power_of_two + 1;
	int fluence_gathering_data_skipped_rays_below_column = (fluence_gathering_data_rays_per_probe + 1) >> 1;
	int fluence_gathering_data_max_ray_probe_row = int(probe_grid_size.y) * fluence_gathering_data_rays_per_probe - (fluence_gathering_data_skipped_rays_below_column << 1) - 1;

	ivec2 output_texel_position = ivec2(merged_to_texel_position);

	// From here we replicate the fluence gather logic.

	int direction_id = output_texel_position.x & fluence_gathering_data_direction_mask;
	int lower_direction_id = direction_id << 1;
	int probe_column = output_texel_position.x >> fluence_gathering_data_cascade;
	int clamped_near_ray_sample_x = min(probe_column, fluence_gathering_data_max_ray_probe_column);
	float near_ray_is_inside = float(probe_column <= fluence_gathering_data_max_ray_probe_column);

	int lower_near_ray_sample_y = 
		output_texel_position.y * fluence_gathering_data_rays_per_probe - fluence_gathering_data_skipped_rays_below_column 
		+ direction_id;

	float cascade_power_of_two_float = float(fluence_gathering_data_cascade_power_of_two); // TODO: Put in uniform block

	// Lower near ray
	int clamped_lower_near_ray_sample_y = clamp(lower_near_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float lower_near_ray_is_inside = near_ray_is_inside * float(0 <= lower_near_ray_sample_y) * float(lower_near_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);
	
	int interpolating = probe_column & 1;

	int lower_y_offset = lower_direction_id - fluence_gathering_data_cascade_power_of_two;	// TODO: This expression is used by lower angle

	int far_ray_sample_x = probe_column + interpolating;
	int clamped_far_ray_sample_x = min(far_ray_sample_x, fluence_gathering_data_max_ray_probe_column);
	float far_ray_is_inside = float(far_ray_sample_x <= fluence_gathering_data_max_ray_probe_column);
	int lower_far_ray_sample_y = lower_near_ray_sample_y + interpolating * lower_y_offset * fluence_gathering_data_rays_per_probe;

	// Lower far ray
	int clamped_lower_far_ray_sample_y = clamp(lower_far_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float lower_far_ray_is_inside = far_ray_is_inside * float(0 <= lower_far_ray_sample_y) * float(lower_far_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);

	float interpolating_float = float(interpolating);

	int upper_near_ray_sample_y = lower_near_ray_sample_y + 1;

	int upper_direction_id = lower_direction_id + 2;
	int upper_y_offset = upper_direction_id - fluence_gathering_data_cascade_power_of_two; // TODO: This expression is used by upper angle

	// Upper near ray
	int clamped_upper_near_ray_sample_y = clamp(upper_near_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float upper_near_ray_is_inside = near_ray_is_inside * float(0 <= upper_near_ray_sample_y) * float(upper_near_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	int upper_far_ray_sample_y = upper_near_ray_sample_y + interpolating * upper_y_offset * fluence_gathering_data_rays_per_probe;

	// Upper far ray
	int clamped_upper_far_ray_sample_y = clamp(upper_far_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float upper_far_ray_is_inside = far_ray_is_inside * float(0 <= upper_far_ray_sample_y) * float(upper_far_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	// End of fluence gather replication

	bool query_is_lower_near_ray = query_texel_position.x == clamped_near_ray_sample_x && query_texel_position.y == clamped_lower_near_ray_sample_y;
	bool query_is_lower_far_ray = query_texel_position.x == clamped_far_ray_sample_x && query_texel_position.y == clamped_lower_far_ray_sample_y;
	
	bool query_is_upper_near_ray = query_texel_position.x == clamped_near_ray_sample_x && query_texel_position.y == clamped_upper_near_ray_sample_y;
	bool query_is_upper_far_ray = query_texel_position.x == clamped_far_ray_sample_x && query_texel_position.y == clamped_upper_far_ray_sample_y;

	const float active_alpha = 1.0;

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	vec4 color = vec4(brightness, brightness, brightness, 0.2);
	if (query_is_lower_near_ray || query_is_lower_far_ray || query_is_upper_near_ray || query_is_upper_far_ray)
	{
		color = vec4(0.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_near_ray)
	{
		color += vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_ray)
	{
		color += vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_near_ray)
	{
		color += vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_far_ray)
	{
		color += vec4(0.0, 1.0, 1.0, active_alpha);
	}
	return color;
}

out vec4 line_color;

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
	uint lines_per_probe = cascade_power_of_two + 1u;
	uint vertices_per_probe = lines_per_probe << 1u;
	uint vertices_per_probe_column = rotated_probe_grid_size.y * vertices_per_probe;
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
		vec2 padding = probe_padding_factor / vec2(source_size);
		vec2 normalized_probe_distance = (2.0 + 2.0 * padding) / vec2(rotated_probe_grid_size - 1u);
		vec2 normalized_position = position * normalized_probe_distance - 1.0 - padding;

		gl_Position = vec4
		(
			normalized_position,
			0.0, 1.0
		);
	#else
		ivec2 direction = ivec2(cascade_power_of_two, int(direction_index << 1u) - int(cascade_power_of_two));
		ivec2 position = ivec2(probe_x, probe_y) + int(gl_VertexID & 1u) * direction;

		vec2 padding = probe_padding_factor / vec2(source_size);
		gl_Position = vec4
		(
			(float(position.x << 1u) + 2.0 * padding.x) / float(rotated_probe_grid_size.x - 1u) - 1.0 - padding,
			(float(position.y << 1u) + 2.0 * padding.y) / float(rotated_probe_grid_size.y - 1u) - 1.0 - padding,
			0.0, 1.0
		);
	#endif

	#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		gl_Position.xy *= 0.5;
	#endif

	#if DIRECTION == EAST_DIRECTION
	#elif DIRECTION == NORTH_DIRECTION
		gl_Position.xy = vec2(-gl_Position.y, gl_Position.x);
	#elif DIRECTION == WEST_DIRECTION
		gl_Position.xy = vec2(-gl_Position.x, -gl_Position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		gl_Position.xy = vec2(-gl_Position.x, gl_Position.y);
	#endif

	#if MODE == SHOWCASE_CASCADE
		float color_factor = float(probe_y & 1u);
		line_color = cascade_colors[cascade % cascade_colors.length()];
		line_color = vec4((1.0 - 2.0 * color_factor) * line_color.rgb + color_factor, 1.0);
	#elif MODE == SHOWCASE_SINGLE_RAY
		#if RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE
			uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
			uvec2 ray_texel_position = convert_ray_logical_to_texel_position
			(
				lines_per_probe, skipped_rays_below_column,
				probe_column, probe_y, direction_index
			);
			const float brightness = 0.2 + float(direction_index & 1u) * 0.8;
			line_color = mix(
				vec4(brightness, 0.0, brightness, 0.2),
				vec4(0.0, 1.0, 0.0, 1.0),
				float(ray_texel_position == showcased_ray_texel_position)
			);
		#elif RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE
			uvec2 ray_texel_position = convert_ray_logical_to_texel_position_in_row_ray_mode(
				lines_per_probe,
				probe_column, probe_y, direction_index
			);
			const float brightness = 0.2 + float(direction_index & 1u) * 0.8;
			line_color = mix(
				vec4(brightness, 0.0, brightness, 0.2),
				vec4(0.0, 1.0, 0.0, 1.0),
				float(ray_texel_position == showcased_ray_texel_position)
			);
		#endif
	#elif MODE == SHOWCASE_MERGE_TO_RAY
		#if RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE
			uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
			line_color = pick_color_if_merging_to_ray
			(
				merged_to_ray_texel_position, 
				lines_per_probe, skipped_rays_below_column, probe_column, probe_y, direction_index
			);
		#elif RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE
			line_color = pick_color_if_merging_to_ray_in_row_ray_mode
			(
				merged_to_ray_texel_position,
				lines_per_probe,
				probe_column, probe_y, direction_index
			);
		#endif
	#elif MODE == SHOWCASE_MERGE_TO_CONE
		#if RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE
			uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
			line_color = pick_color_if_merging_to_cone
			(
				merged_to_cone_texel_position, 
				lines_per_probe, skipped_rays_below_column, probe_column, probe_y, direction_index
			);
		#elif RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE
			line_color = pick_color_if_merging_to_cone_in_row_ray_mode
			(
				merged_to_cone_texel_position, 
				lines_per_probe, probe_column, probe_y, direction_index
			);
		#endif
	#elif MODE == SHOWCASE_RADIANCE
		#if RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE
			uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
			uvec2 ray_texel_position = convert_ray_logical_to_texel_position
			(
				lines_per_probe, skipped_rays_below_column,
				probe_column, probe_y, direction_index
			);
		#elif RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE
			uvec2 ray_texel_position = convert_ray_logical_to_texel_position_in_row_ray_mode(
				lines_per_probe,
				probe_column, probe_y, direction_index
			);
		#endif
		if
		(
			0 <= ray_texel_position.x && ray_texel_position.x < textureSize(rays, 0).x &&
			0 <= ray_texel_position.y && ray_texel_position.y < textureSize(rays, 0).y
		)
		{
			line_color = texelFetch(rays, ivec3(ray_texel_position, 0), 0);
			line_color.a = 1.0;
		}
		else
		{
			line_color = vec4(1.0, 0.0, 0.0, 1.0);
		}
	#endif
}
