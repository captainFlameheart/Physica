/* Expected to be concatenated from CPU:

#define COLUMN_RAY_TEXTURE_MODE ?
#define ROW_RAY_TEXTURE_MODE ?

#define RAY_TEXTURE_MODE ?

const uvec2 max_fluence_texture_xy;
const uvec2 max_ray_texture_xy;
const uvec2 max_upper_cascade_fluence_texture_xy;

*/

#define EAST_DIRECTION 0
#define NORTH_DIRECTION 1
#define WEST_DIRECTION 2
#define SOUTH_DIRECTION 3

#define DIRECTION EAST_DIRECTION

// IMPORTANT TODO: Only store data necessary for the particular ray texture mode.
layout(shared, binding = FLUENCE_GATHERING_BINDING) uniform Fluence_Gathering_Data
{
	int direction_mask;
	int cascade;
	int max_ray_probe_column;
	int max_ray_probe_row;
	int max_fluence_probe_column_texel_x;
	int max_fluence_probe_y;
	int rays_per_probe;
	int skipped_rays_below_column;
	int cascade_power_of_two;
	int upper_cascade_probe_column_texel_x_mask;
	int upper_cascade;
	int upper_cascade_fluence_layer;
} fluence_gathering_data;

uniform sampler2DArray rays;
uniform sampler2DArray upper_cascade_fluence;

layout (location = 0) out vec4 fluence;

ivec2 logical_to_physical_ray_texel_position(in ivec2 logical_position)
{
	#if DIRECTION == EAST_DIRECTION
		return logical_position;
	#elif DIRECTION == NORTH_DIRECTION
		return ivec2(max_ray_texture_xy.x - logical_position.y, logical_position.x);
	#elif DIRECTION == WEST_DIRECTION
		return ivec2(max_ray_texture_xy.x - logical_position.x, max_ray_texture_xy.y - logical_position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		return ivec2(logical_position.y, max_ray_texture_xy.y - logical_position.x);
	#endif
}

ivec2 logical_to_physical_upper_cascade_fluence_texel_position(in ivec2 logical_position)
{
	#if DIRECTION == EAST_DIRECTION
		return logical_position;
	#elif DIRECTION == NORTH_DIRECTION
		return ivec2(max_upper_cascade_fluence_texture_xy.x - logical_position.y, logical_position.x);
	#elif DIRECTION == WEST_DIRECTION
		return ivec2(max_upper_cascade_fluence_texture_xy.x - logical_position.x, max_upper_cascade_fluence_texture_xy.y - logical_position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		return ivec2(logical_position.y, max_upper_cascade_fluence_texture_xy.y - logical_position.x);
	#endif
}

void main()
{
	// IMPORTANT TODO: Angles should depend on probe grid dimensions and whatnot.

	#if RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE

		// IMPORTANT TODO: REORDER TEXTURE FETCHES BASED ON CHACHE LOCALITY
		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		#if DIRECTION == EAST_DIRECTION
		#elif DIRECTION == NORTH_DIRECTION
			output_texel_position = ivec2(output_texel_position.y, max_fluence_texture_xy.x - output_texel_position.x);
		#elif DIRECTION == WEST_DIRECTION
			output_texel_position = ivec2(max_fluence_texture_xy.x - output_texel_position.x, max_fluence_texture_xy.y - output_texel_position.y);
		#elif DIRECTION == SOUTH_DIRECTION
			output_texel_position = ivec2(max_fluence_texture_xy.y - output_texel_position.y, output_texel_position.x);
		#endif

		int direction_id = output_texel_position.x & fluence_gathering_data.direction_mask;
		int lower_direction_id = direction_id << 1;
		int probe_column = output_texel_position.x >> fluence_gathering_data.cascade;
	
		int clamped_near_ray_column_texel_x = min(probe_column, fluence_gathering_data.max_ray_probe_column) * fluence_gathering_data.rays_per_probe;
		float near_ray_is_inside = float(probe_column <= fluence_gathering_data.max_ray_probe_column);

		float cascade_power_of_two_float = float(fluence_gathering_data.cascade_power_of_two); // TODO: Put in uniform block
		float middle_angle = atan(float((lower_direction_id + 1) - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float);

		float lower_angle = middle_angle - atan(float(lower_direction_id - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float);

		// Lower near ray
		int clamped_lower_near_ray_texel_x = clamped_near_ray_column_texel_x + direction_id;
		ivec2 physical_lower_near_ray_texel_position = logical_to_physical_ray_texel_position(ivec2(clamped_lower_near_ray_texel_x, output_texel_position.y));
		fluence = (lower_angle * near_ray_is_inside) * texelFetch(rays, ivec3(physical_lower_near_ray_texel_position, 0), 0);
		vec4 lower_near_transmittance = mix
		(
			vec4(1.0), texelFetch(rays, ivec3(physical_lower_near_ray_texel_position, 1), 0),
			near_ray_is_inside
		);
	
		vec4 shifted_lower_near_transmittance = lower_near_transmittance - 1.0;

		int interpolating = probe_column & 1;

		int lower_y_offset = lower_direction_id - fluence_gathering_data.cascade_power_of_two;	// TODO: This expression is used by lower angle

		int far_ray_column = probe_column + interpolating;
		int clamped_far_ray_column_texel_x = min(far_ray_column, fluence_gathering_data.max_ray_probe_column) * fluence_gathering_data.rays_per_probe;
		float far_ray_x_is_inside = float(far_ray_column <= fluence_gathering_data.max_ray_probe_column);

		// Lower far ray
		int clamped_lower_far_ray_texel_x = clamped_far_ray_column_texel_x + direction_id;
		int clamped_lower_far_ray_texel_y = clamp(output_texel_position.y + interpolating * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);
		ivec2 physical_lower_far_ray_texel_position = logical_to_physical_ray_texel_position(ivec2(clamped_lower_far_ray_texel_x, clamped_lower_far_ray_texel_y));
		vec4 lower_far_fluence = (lower_angle * far_ray_x_is_inside) * texelFetch(rays, ivec3(physical_lower_far_ray_texel_position, 0), 0);
		vec4 lower_far_transmittance = mix(
			vec4(1.0), texelFetch(rays, ivec3(physical_lower_far_ray_texel_position, 1), 0), 
			far_ray_x_is_inside
		);

		int not_interpolating = interpolating ^ 1;
		float not_interpolating_float = float(not_interpolating);
		vec4 lower_near_transmit_factor = 1.0 + not_interpolating_float * shifted_lower_near_transmittance;

		// IMPORTANT TODO: The clamping should not be necessary here, but it might be necessary when the fluence is added to the final fluence 
		// buffer.
		int near_sample_probe_column_texel_x = min(
			output_texel_position.x & fluence_gathering_data.upper_cascade_probe_column_texel_x_mask, fluence_gathering_data.max_fluence_probe_column_texel_x
		);
		int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_direction_id;
		int lower_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

		// Lower near fluence
		ivec2 lower_near_fluence_texel_position = logical_to_physical_upper_cascade_fluence_texel_position(ivec2(lower_near_fluence_sample_x, lower_near_fluence_sample_y));
		fluence += lower_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(lower_near_fluence_texel_position, fluence_gathering_data.upper_cascade_fluence_layer), 0);

		int lower_far_fluence_sample_x = min(
			near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data.upper_cascade), fluence_gathering_data.max_fluence_probe_column_texel_x
		) + lower_direction_id;
		int lower_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

		// Lower far fluence
		ivec2 lower_far_fluence_texel_position = logical_to_physical_upper_cascade_fluence_texel_position(ivec2(lower_far_fluence_sample_x, lower_far_fluence_sample_y));
		lower_far_fluence += lower_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(lower_far_fluence_texel_position, fluence_gathering_data.upper_cascade_fluence_layer), 0);

		float interpolating_float = float(interpolating);
		vec4 lower_far_transmit_factor = 1.0 + interpolating_float * shifted_lower_near_transmittance;
		fluence += lower_far_transmit_factor * lower_far_fluence;

		int upper_direction_id = lower_direction_id + 2;
		int upper_y_offset = upper_direction_id - fluence_gathering_data.cascade_power_of_two; // TODO: This expression is used by upper angle

		float upper_angle = atan(float(upper_direction_id - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float) - middle_angle;

		// Upper near ray
		int clamped_upper_near_ray_texel_x = clamped_lower_near_ray_texel_x + 1;
		ivec2 physical_upper_near_ray_texel_position = logical_to_physical_ray_texel_position(ivec2(clamped_upper_near_ray_texel_x, output_texel_position.y));
		fluence += (upper_angle * near_ray_is_inside) * texelFetch(rays, ivec3(physical_upper_near_ray_texel_position, 0), 0);
		vec4 upper_near_transmittance = mix(
			vec4(1.0), texelFetch(rays, ivec3(physical_upper_near_ray_texel_position, 1), 0), 
			near_ray_is_inside
		);

		vec4 shifted_upper_near_transmittance = upper_near_transmittance - 1.0;

		// Upper far ray
		int clamped_upper_far_ray_texel_x = clamped_lower_far_ray_texel_x + 1;
		int clamped_upper_far_ray_texel_y = clamp(output_texel_position.y + interpolating * upper_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);
		ivec2 physical_upper_far_ray_texel_position = logical_to_physical_ray_texel_position(ivec2(clamped_upper_far_ray_texel_x, clamped_upper_far_ray_texel_y));
		vec4 upper_far_fluence = (upper_angle * far_ray_x_is_inside) * texelFetch(rays, ivec3(physical_upper_far_ray_texel_position, 0), 0);
		vec4 upper_far_transmittance = mix(
			vec4(1.0), texelFetch(rays, ivec3(physical_upper_far_ray_texel_position, 1), 0), 
			far_ray_x_is_inside
		);

		vec4 upper_near_transmit_factor = 1.0 + not_interpolating_float * shifted_upper_near_transmittance;

		int upper_near_fluence_sample_x = near_sample_probe_column_texel_x + upper_direction_id - 1;
		int upper_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * upper_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

		// Upper near fluence
		ivec2 upper_near_fluence_texel_position = logical_to_physical_upper_cascade_fluence_texel_position(ivec2(upper_near_fluence_sample_x, upper_near_fluence_sample_y));
		fluence += upper_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(upper_near_fluence_texel_position, fluence_gathering_data.upper_cascade_fluence_layer), 0);

		int upper_far_fluence_sample_x = min(
			near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data.upper_cascade), fluence_gathering_data.max_fluence_probe_column_texel_x
		) + upper_direction_id - 1;	// TODO: Many expressions that have been evaluated earlier
		int upper_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * upper_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);
	
		// Upper far fluence
		ivec2 upper_far_fluence_texel_position = logical_to_physical_upper_cascade_fluence_texel_position(ivec2(upper_far_fluence_sample_x, upper_far_fluence_sample_y));
		upper_far_fluence += upper_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(upper_far_fluence_texel_position, fluence_gathering_data.upper_cascade_fluence_layer), 0);

		vec4 upper_far_transmit_factor = 1.0 + interpolating_float * shifted_upper_near_transmittance;
		fluence += upper_far_transmit_factor * upper_far_fluence;

		fluence *= 0.5;

	#elif RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE

	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int direction_id = output_texel_position.x & fluence_gathering_data.direction_mask;
	int lower_direction_id = direction_id << 1;
	int probe_column = output_texel_position.x >> fluence_gathering_data.cascade;
	int clamped_near_ray_sample_x = min(probe_column, fluence_gathering_data.max_ray_probe_column);
	float near_ray_is_inside = float(probe_column <= fluence_gathering_data.max_ray_probe_column);

	int lower_near_ray_sample_y = 
		output_texel_position.y * fluence_gathering_data.rays_per_probe - fluence_gathering_data.skipped_rays_below_column 
		+ direction_id;

	float cascade_power_of_two_float = float(fluence_gathering_data.cascade_power_of_two); // TODO: Put in uniform block
	float middle_angle = atan(float((lower_direction_id + 1) - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float);

	float lower_angle = middle_angle - atan(float(lower_direction_id - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float);

	// Lower near ray
	int clamped_lower_near_ray_sample_y = clamp(lower_near_ray_sample_y, 0, fluence_gathering_data.max_ray_probe_row);
	float lower_near_ray_is_inside = near_ray_is_inside * float(0 <= lower_near_ray_sample_y) * float(lower_near_ray_sample_y <= fluence_gathering_data.max_ray_probe_row);
	fluence = (lower_angle * lower_near_ray_is_inside) * texelFetch(rays, ivec3(clamped_near_ray_sample_x, clamped_lower_near_ray_sample_y, 0), 0);
	vec4 lower_near_transmittance = mix(
		vec4(1.0), lower_near_ray_is_inside * texelFetch(rays, ivec3(clamped_near_ray_sample_x, clamped_lower_near_ray_sample_y, 1), 0), 
		lower_near_ray_is_inside
	);
	
	vec4 shifted_lower_near_transmittance = lower_near_transmittance - 1.0;

	int interpolating = probe_column & 1;

	int lower_y_offset = lower_direction_id - fluence_gathering_data.cascade_power_of_two;	// TODO: This expression is used by lower angle

	int far_ray_sample_x = probe_column + interpolating;
	int clamped_far_ray_sample_x = min(far_ray_sample_x, fluence_gathering_data.max_ray_probe_column);
	float far_ray_is_inside = float(far_ray_sample_x <= fluence_gathering_data.max_ray_probe_column);
	int lower_far_ray_sample_y = lower_near_ray_sample_y + interpolating * lower_y_offset * fluence_gathering_data.rays_per_probe;

	// Lower far ray
	int clamped_lower_far_ray_sample_y = clamp(lower_far_ray_sample_y, 0, fluence_gathering_data.max_ray_probe_row);
	float lower_far_ray_is_inside = far_ray_is_inside * float(0 <= lower_far_ray_sample_y) * float(lower_far_ray_sample_y <= fluence_gathering_data.max_ray_probe_row);
	vec4 lower_far_fluence = (lower_angle * lower_far_ray_is_inside) * texelFetch(rays, ivec3(clamped_far_ray_sample_x, clamped_lower_far_ray_sample_y, 0), 0);
	vec4 lower_far_transmittance = mix(
		vec4(1.0), texelFetch(rays, ivec3(clamped_far_ray_sample_x, clamped_lower_far_ray_sample_y, 1), 0), 
		lower_far_ray_is_inside
	);

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);
	vec4 lower_near_transmit_factor = 1.0 + not_interpolating_float * shifted_lower_near_transmittance;

	// IMPORTANT TODO: The clamping should not be necessary here, but it might be necessary when the fluence is added to the final fluence 
	// buffer.
	int near_sample_probe_column_texel_x = min(
		output_texel_position.x & fluence_gathering_data.upper_cascade_probe_column_texel_x_mask, fluence_gathering_data.max_fluence_probe_column_texel_x
	);
	int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_direction_id;
	int lower_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

	// Lower near fluence
	fluence += lower_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(lower_near_fluence_sample_x, lower_near_fluence_sample_y, fluence_gathering_data.upper_cascade_fluence_layer), 0);

	int lower_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data.upper_cascade), fluence_gathering_data.max_fluence_probe_column_texel_x
	) + lower_direction_id;
	int lower_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

	// Lower far fluence
	lower_far_fluence += lower_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(lower_far_fluence_sample_x, lower_far_fluence_sample_y, fluence_gathering_data.upper_cascade_fluence_layer), 0);

	float interpolating_float = float(interpolating);
	vec4 lower_far_transmit_factor = 1.0 + interpolating_float * shifted_lower_near_transmittance;
	fluence += lower_far_transmit_factor * lower_far_fluence;

	int upper_near_ray_sample_y = lower_near_ray_sample_y + 1;

	int upper_direction_id = lower_direction_id + 2;
	int upper_y_offset = upper_direction_id - fluence_gathering_data.cascade_power_of_two; // TODO: This expression is used by upper angle

	float upper_angle = atan(float(upper_direction_id - fluence_gathering_data.cascade_power_of_two) / cascade_power_of_two_float) - middle_angle;

	// Upper near ray
	int clamped_upper_near_ray_sample_y = clamp(upper_near_ray_sample_y, 0, fluence_gathering_data.max_ray_probe_row);
	float upper_near_ray_is_inside = near_ray_is_inside * float(0 <= upper_near_ray_sample_y) * float(upper_near_ray_sample_y <= fluence_gathering_data.max_ray_probe_row);
	fluence += (upper_angle * upper_near_ray_is_inside) * texelFetch(rays, ivec3(clamped_near_ray_sample_x, clamped_upper_near_ray_sample_y, 0), 0);
	vec4 upper_near_transmittance = mix(
		vec4(1.0), texelFetch(rays, ivec3(clamped_near_ray_sample_x, clamped_upper_near_ray_sample_y, 1), 0), 
		upper_near_ray_is_inside
	);
	vec4 shifted_upper_near_transmittance = upper_near_transmittance - 1.0;

	int upper_far_ray_sample_y = upper_near_ray_sample_y + interpolating * upper_y_offset * fluence_gathering_data.rays_per_probe;

	// Upper far ray
	int clamped_upper_far_ray_sample_y = clamp(upper_far_ray_sample_y, 0, fluence_gathering_data.max_ray_probe_row);
	float upper_far_ray_is_inside = far_ray_is_inside * float(0 <= upper_far_ray_sample_y) * float(upper_far_ray_sample_y <= fluence_gathering_data.max_ray_probe_row);
	vec4 upper_far_fluence = (upper_angle * upper_far_ray_is_inside) * texelFetch(rays, ivec3(clamped_far_ray_sample_x, clamped_upper_far_ray_sample_y, 0), 0);
	vec4 upper_far_transmittance = mix(
		vec4(1.0), texelFetch(rays, ivec3(clamped_far_ray_sample_x, clamped_upper_far_ray_sample_y, 1), 0), 
		upper_far_ray_is_inside
	);

	vec4 upper_near_transmit_factor = 1.0 + not_interpolating_float * shifted_upper_near_transmittance;

	int upper_near_fluence_sample_x = near_sample_probe_column_texel_x + upper_direction_id - 1;
	int upper_near_fluence_sample_y = clamp(output_texel_position.y + not_interpolating * upper_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

	// Upper near fluence
	fluence += upper_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec3(upper_near_fluence_sample_x, upper_near_fluence_sample_y, fluence_gathering_data.upper_cascade_fluence_layer), 0);

	//int lower_far_fluence_sample_x = min(
	//	near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data.upper_cascade), fluence_gathering_data.max_fluence_probe_column_texel_x
	//) + lower_direction_id;
	//int lower_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * lower_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);

	int upper_far_fluence_sample_x = min(
		near_sample_probe_column_texel_x + (interpolating << fluence_gathering_data.upper_cascade), fluence_gathering_data.max_fluence_probe_column_texel_x
	) + upper_direction_id - 1;	// TODO: Many expressions that have been evaluated earlier
	int upper_far_fluence_sample_y = clamp(output_texel_position.y + (interpolating + 1) * upper_y_offset, 0, fluence_gathering_data.max_fluence_probe_y);
	
	//int upper_far_fluence_sample_x = upper_near_fluence_sample_x + (interpolating << fluence_gathering_data.upper_cascade);
	//int upper_far_fluence_sample_y = output_texel_position.y + (interpolating + 1) * upper_y_offset;

	// Upper far fluence
	upper_far_fluence += upper_far_transmittance * texelFetch(upper_cascade_fluence, ivec3(upper_far_fluence_sample_x, upper_far_fluence_sample_y, fluence_gathering_data.upper_cascade_fluence_layer), 0);

	vec4 upper_far_transmit_factor = 1.0 + interpolating_float * shifted_upper_near_transmittance;
	fluence += upper_far_transmit_factor * upper_far_fluence;

	fluence *= 0.5;

	#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	#if 1 == 0
		fluence = texelFetch(rays, ivec3(clamped_near_ray_sample_x, clamped_lower_near_ray_sample_y, 0), 0);
		//fluence = vec4(1.0, 0.0, 1.0, 1.0);
		//fluence *= 1000.0;
	#endif
}
