/* Expected to be concatenated from CPU:

#define DIRECTION ?

#define COLLAPSE_DISTANCE_CONES ?

*/

#if DIRECTION == EAST_DIRECTION || DIRECTION == WEST_DIRECTION
	
	#define X(v) ((v).x)
	#define Y(v) ((v).y)

	#define IVEC2(X, Y) ivec2(X, Y)
	#define IVEC3(X, Y, Z) ivec3(X, Y, Z)
	#define VEC2(X, Y) vec2(X, Y)

#elif DIRECTION == NORTH_DIRECTION || DIRECTION == SOUTH_DIRECTION
	
	#define X(v) ((v).y)
	#define Y(v) ((v).x)

	#define IVEC2(X, Y) ivec2(Y, X)
	#define IVEC3(X, Y, Z) ivec3(Y, X, Z)
	#define VEC2(X, Y) vec2(Y, X)

#endif

layout(location = rays_texture_unit) uniform sampler2DArray rays;
layout(location = upper_cascade_texture_unit) uniform sampler2DArray upper_cascade_fluence;

layout(location = 0) out vec4 fluence;

void main()
{
	// IMPORTANT TODO: Angles should depend on probe grid dimensions and whatnot.

	// IMPORTANT TODO: REORDER TEXTURE FETCHES BASED ON CHACHE LOCALITY
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	#if DIRECTION == WEST_DIRECTION
		output_texel_position.x = merge_fluence_data.output_factor * output_texel_position.x + merge_fluence_data.output_shift;
	#elif DIRECTION == SOUTH_DIRECTION
		output_texel_position.y = merge_fluence_data.output_factor * output_texel_position.y + merge_fluence_data.output_shift;
	#endif

	int direction_id = X(output_texel_position) & merge_fluence_data.direction_mask;
	int lower_cone_direction_id = direction_id << 1;
	int probe_column = X(output_texel_position) >> merge_fluence_data.cascade;
	
	int clamped_near_ray_column_texel_x = min(probe_column, merge_fluence_data.max_ray_probe_column) * merge_fluence_data.rays_per_probe;
	float near_ray_is_inside = float(probe_column <= merge_fluence_data.max_ray_probe_column);

	float cascade_power_of_two_float = float(merge_fluence_data.cascade_power_of_two); // TODO: Put in uniform block
	float middle_angle = atan(float((lower_cone_direction_id + 1) - merge_fluence_data.cascade_power_of_two) / cascade_power_of_two_float);

	float lower_angle = middle_angle - atan(float(lower_cone_direction_id - merge_fluence_data.cascade_power_of_two) / cascade_power_of_two_float);

	// Lower near ray
	int clamped_lower_near_ray_texel_x = clamped_near_ray_column_texel_x + direction_id;
	fluence = (lower_angle * near_ray_is_inside) * texelFetch(rays, IVEC3(clamped_lower_near_ray_texel_x, Y(output_texel_position), 0), 0);
	vec4 lower_near_transmittance = mix
	(
		vec4(1.0), texelFetch(rays, IVEC3(clamped_lower_near_ray_texel_x, Y(output_texel_position), 1), 0),
		near_ray_is_inside
	);

	int upper_ray_y_steps = lower_cone_direction_id + 2;

	float upper_angle = atan(float(upper_ray_y_steps - merge_fluence_data.cascade_power_of_two) / cascade_power_of_two_float) - middle_angle;

	// Upper near ray
	int clamped_upper_near_ray_texel_x = clamped_lower_near_ray_texel_x + 1;
	fluence += (upper_angle * near_ray_is_inside) * texelFetch(rays, IVEC3(clamped_upper_near_ray_texel_x, Y(output_texel_position), 0), 0);
	vec4 upper_near_transmittance = mix(
		vec4(1.0), texelFetch(rays, IVEC3(clamped_upper_near_ray_texel_x, Y(output_texel_position), 1), 0), 
		near_ray_is_inside
	);
	
	int interpolating = probe_column & 1;

	int lower_y_offset = lower_cone_direction_id - merge_fluence_data.cascade_power_of_two;	// TODO: This expression is used by lower angle

	int far_ray_column = probe_column + interpolating;
	int clamped_far_ray_column_texel_x = min(far_ray_column, merge_fluence_data.max_ray_probe_column) * merge_fluence_data.rays_per_probe;
	float far_ray_x_is_inside = float(far_ray_column <= merge_fluence_data.max_ray_probe_column);

	// Lower far ray
	int clamped_lower_far_ray_texel_x = clamped_far_ray_column_texel_x + direction_id;
	int clamped_lower_far_ray_texel_y = clamp(Y(output_texel_position) + interpolating * lower_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	vec4 lower_far_fluence = (lower_angle * far_ray_x_is_inside) * texelFetch(rays, IVEC3(clamped_lower_far_ray_texel_x, clamped_lower_far_ray_texel_y, 0), 0);
	vec4 lower_far_transmittance = mix(
		vec4(1.0), texelFetch(rays, IVEC3(clamped_lower_far_ray_texel_x, clamped_lower_far_ray_texel_y, 1), 0), 
		far_ray_x_is_inside
	);

	int upper_y_offset = upper_ray_y_steps - merge_fluence_data.cascade_power_of_two; // TODO: This expression is used by upper angle

	// Upper far ray
	int clamped_upper_far_ray_texel_x = clamped_lower_far_ray_texel_x + 1;
	int clamped_upper_far_ray_texel_y = clamp(Y(output_texel_position) + interpolating * upper_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	vec4 upper_far_fluence = (upper_angle * far_ray_x_is_inside) * texelFetch(rays, IVEC3(clamped_upper_far_ray_texel_x, clamped_upper_far_ray_texel_y, 0), 0);
	vec4 upper_far_transmittance = mix(
		vec4(1.0), texelFetch(rays, IVEC3(clamped_upper_far_ray_texel_x, clamped_upper_far_ray_texel_y, 1), 0), 
		far_ray_x_is_inside
	);

	vec4 shifted_lower_near_transmittance = lower_near_transmittance - 1.0;

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);
	vec4 lower_near_transmit_factor = 1.0 + not_interpolating_float * shifted_lower_near_transmittance;

	int near_sample_probe_column_texel_x = min(
		X(output_texel_position) & merge_fluence_data.upper_cascade_probe_column_texel_x_mask, merge_fluence_data.max_fluence_probe_column_texel_x
	);
	#if COLLAPSE_DISTANCE_CONES == 1
		int near_fluence_is_inside = int(near_sample_probe_column_texel_x != merge_fluence_data.max_fluence_probe_column_texel_x);
	#endif

	int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_cone_direction_id;
	int lower_near_fluence_sample_y = clamp(Y(output_texel_position) + not_interpolating * lower_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	#if COLLAPSE_DISTANCE_CONES == 1
		lower_near_fluence_sample_y *= near_fluence_is_inside;
	#endif

	// Lower near fluence
	fluence += lower_near_transmit_factor * texelFetch(upper_cascade_fluence, IVEC3(lower_near_fluence_sample_x, lower_near_fluence_sample_y, merge_fluence_data.upper_cascade_fluence_layer), 0);

	vec4 shifted_upper_near_transmittance = upper_near_transmittance - 1.0;
		
	vec4 upper_near_transmit_factor = 1.0 + not_interpolating_float * shifted_upper_near_transmittance;

	int upper_near_fluence_sample_x = lower_near_fluence_sample_x + 1;
	int upper_near_fluence_sample_y = clamp(Y(output_texel_position) + not_interpolating * upper_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	#if COLLAPSE_DISTANCE_CONES == 1
		upper_near_fluence_sample_y *= near_fluence_is_inside;
	#endif

	// Upper near fluence
	fluence += upper_near_transmit_factor * texelFetch(upper_cascade_fluence, IVEC3(upper_near_fluence_sample_x, upper_near_fluence_sample_y, merge_fluence_data.upper_cascade_fluence_layer), 0);

	int far_fluence_probe_column_texel_x = min(
		near_sample_probe_column_texel_x + (interpolating << merge_fluence_data.upper_cascade), merge_fluence_data.max_fluence_probe_column_texel_x
	);
	#if COLLAPSE_DISTANCE_CONES == 1
		int far_fluence_is_inside = int(far_fluence_probe_column_texel_x != merge_fluence_data.max_fluence_probe_column_texel_x);
	#endif
		
	int lower_far_fluence_sample_x = far_fluence_probe_column_texel_x + lower_cone_direction_id;
	int lower_far_fluence_sample_y = clamp(Y(output_texel_position) + (interpolating + 1) * lower_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	#if COLLAPSE_DISTANCE_CONES == 1
		lower_far_fluence_sample_y *= far_fluence_is_inside;
	#endif

	// Lower far fluence
	lower_far_fluence += lower_far_transmittance * texelFetch(upper_cascade_fluence, IVEC3(lower_far_fluence_sample_x, lower_far_fluence_sample_y, merge_fluence_data.upper_cascade_fluence_layer), 0);

	float interpolating_float = float(interpolating);
	vec4 lower_far_transmit_factor = 1.0 + interpolating_float * shifted_lower_near_transmittance;
	fluence += lower_far_transmit_factor * lower_far_fluence;

	int upper_far_fluence_sample_x = lower_far_fluence_sample_x + 1;	// TODO: Many expressions that have been evaluated earlier
	int upper_far_fluence_sample_y = clamp(Y(output_texel_position) + (interpolating + 1) * upper_y_offset, 0, merge_fluence_data.max_fluence_probe_y);
	#if COLLAPSE_DISTANCE_CONES == 1
		upper_far_fluence_sample_y *= far_fluence_is_inside;
	#endif

	// Upper far fluence
	upper_far_fluence += upper_far_transmittance * texelFetch(upper_cascade_fluence, IVEC3(upper_far_fluence_sample_x, upper_far_fluence_sample_y, merge_fluence_data.upper_cascade_fluence_layer), 0);

	vec4 upper_far_transmit_factor = 1.0 + interpolating_float * shifted_upper_near_transmittance;
	fluence += upper_far_transmit_factor * upper_far_fluence;

	fluence *= 0.5;
}
