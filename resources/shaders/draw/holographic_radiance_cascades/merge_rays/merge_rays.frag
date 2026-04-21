/* Expected to be concatenated from CPU:

#define DIRECTION ?

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

layout(binding = rays_texture_unit) uniform sampler2DArray shorter_rays;

layout(location = rays_radiance_layer) out vec4 radiance;
layout(location = rays_transmittance_layer) out vec4 transmittance;

void main()
{
	/*radiance = vec4(1.0, 0.0, 0.0, 0.0);
	transmittance = vec4(1.0);
	return;*/

	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);

	int probe_column = X(output_texel_position) / merge_rays_data.rays_per_probe;
	int probe_column_texel_x = probe_column * merge_rays_data.rays_per_probe;
	int direction_id = X(output_texel_position) - probe_column_texel_x;

	int lower_cascade_near_probe_column_texel_x = 
		((probe_column << 1) | 1) * merge_rays_data.lower_cascade_rays_per_probe;
	
	int lower_direction_id = direction_id >> 1;

	// Lower near
	int lower_near_texel_x = lower_cascade_near_probe_column_texel_x + lower_direction_id;	// Does not need clamping.
	radiance = texelFetch(shorter_rays, IVEC3(lower_near_texel_x, Y(output_texel_position), 0), 0);
	transmittance = texelFetch(shorter_rays, IVEC3(lower_near_texel_x, Y(output_texel_position), 1), 0);
	
	int upper_direction_id = (direction_id + 1) >> 1;

	// Upper near
	int upper_near_texel_x = lower_cascade_near_probe_column_texel_x + upper_direction_id;	// Does not need clamping.
	radiance += texelFetch(shorter_rays, IVEC3(upper_near_texel_x, Y(output_texel_position), 0), 0);
	vec4 upper_transmittance = texelFetch(shorter_rays, IVEC3(upper_near_texel_x, Y(output_texel_position), 1), 0);

	int lower_cascade_far_probe_column_texel_x = 
		lower_cascade_near_probe_column_texel_x + merge_rays_data.lower_cascade_rays_per_probe;
	int clamped_lower_cascade_far_probe_column_texel_x = 
		min(lower_cascade_far_probe_column_texel_x, merge_rays_data.lower_cascade_max_probe_column_texel_x);
	float lower_cascade_far_probe_column_is_inside_bounds = float
	(
		lower_cascade_far_probe_column_texel_x <= merge_rays_data.lower_cascade_max_probe_column_texel_x
	);
	int far_base_y = Y(output_texel_position) - merge_rays_data.lower_cascade_power_of_two;

	// Lower far
	int clamped_lower_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + upper_direction_id;
	int lower_far_texel_y = far_base_y + (lower_direction_id << 1);
	int clamped_lower_far_texel_y = clamp(lower_far_texel_y, 0, merge_rays_data.lower_cascade_max_probe_row);
	radiance += transmittance * 
		(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, IVEC3(clamped_lower_far_texel_x, clamped_lower_far_texel_y, 0), 0));
	transmittance *= mix
	(
		vec4(1.0),
		texelFetch(shorter_rays, IVEC3(clamped_lower_far_texel_x, clamped_lower_far_texel_y, 1), 0),
		lower_cascade_far_probe_column_is_inside_bounds
	);

	// Upper far
	int clamped_upper_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + lower_direction_id;
	int upper_far_texel_y = far_base_y + (upper_direction_id << 1);
	int clamped_upper_far_texel_y = clamp(upper_far_texel_y, 0, merge_rays_data.lower_cascade_max_probe_row);
	radiance += upper_transmittance * 
		(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, IVEC3(clamped_upper_far_texel_x, clamped_upper_far_texel_y, 0), 0));
	upper_transmittance *= mix
	(
		vec4(1.0),
		texelFetch(shorter_rays, IVEC3(clamped_upper_far_texel_x, clamped_upper_far_texel_y, 1), 0),
		lower_cascade_far_probe_column_is_inside_bounds
	);

	transmittance += upper_transmittance;

	radiance *= 0.5;
	transmittance *= 0.5;
}
