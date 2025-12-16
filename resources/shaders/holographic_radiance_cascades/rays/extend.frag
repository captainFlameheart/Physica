/* Expected to be concatenated from CPU:

#define COLUMN_RAY_TEXTURE_MODE ?
#define ROW_RAY_TEXTURE_MODE ?

#define RAY_TEXTURE_MODE ?

*/

#define EAST_DIRECTION 0
#define NORTH_DIRECTION 1
#define WEST_DIRECTION 2
#define SOUTH_DIRECTION 3

#define DIRECTION EAST_DIRECTION

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

// IMPORTANT TODO: Only store data necessary for the particular ray texture mode.
layout(shared, binding = RAY_CASTING_BINDING) uniform Ray_Casting_Data
{
	int skipped_rays_below_column;
	int rays_per_probe;
	int g;
	int f;
	int lower_cascade_rays_per_probe;
	int lower_cascade_skipped_rays_below_column;
	int lower_cascade_max_ray_probe_column;
	int lower_cascade_max_ray_probe_row;

	int lower_cascade_power_of_two;
	int lower_cascade_max_probe_column_texel_x;
	int lower_cascade_max_probe_row;

	ivec2 max_ray_texture_xy;
	ivec2 max_lower_cascade_ray_texture_xy;
} ray_casting_data;

uniform sampler2DArray shorter_rays;

layout (location = 0) out vec4 radiance;
layout (location = 1) out vec4 transmittance;

ivec2 logical_to_physical_lower_cascade_ray_texel_position(in ivec2 logical_position)
{
	#if DIRECTION == EAST_DIRECTION
		return logical_position;
	#elif DIRECTION == NORTH_DIRECTION
		return ivec2(ray_casting_data.max_lower_cascade_ray_texture_xy.x - logical_position.y, logical_position.x);
	#elif DIRECTION == WEST_DIRECTION
		return ivec2(ray_casting_data.max_lower_cascade_ray_texture_xy.x - logical_position.x, ray_casting_data.max_lower_cascade_ray_texture_xy.y - logical_position.y);
	#elif DIRECTION == SOUTH_DIRECTION
		return ivec2(logical_position.y, ray_casting_data.max_lower_cascade_ray_texture_xy.y - logical_position.x);
	#endif
}

void main()
{
	#if RAY_TEXTURE_MODE == ROW_RAY_TEXTURE_MODE

		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		#if DIRECTION == EAST_DIRECTION
		#elif DIRECTION == NORTH_DIRECTION
			output_texel_position = ivec2(output_texel_position.y, ray_casting_data.max_ray_texture_xy.x - output_texel_position.x);
		#elif DIRECTION == WEST_DIRECTION
			output_texel_position = ivec2(ray_casting_data.max_ray_texture_xy.x - output_texel_position.x, ray_casting_data.max_ray_texture_xy.y - output_texel_position.y);
		#elif DIRECTION == SOUTH_DIRECTION
			output_texel_position = ivec2(ray_casting_data.max_ray_texture_xy.y - output_texel_position.y, output_texel_position.x);
		#endif

		int probe_column = output_texel_position.x / ray_casting_data.rays_per_probe;
		int probe_column_texel_x = probe_column * ray_casting_data.rays_per_probe;
		int direction_id = output_texel_position.x - probe_column_texel_x;

		int lower_cascade_near_probe_column_texel_x = 
			((probe_column << 1) | 1) * ray_casting_data.lower_cascade_rays_per_probe;
	
		int lower_direction_id = direction_id >> 1;

		// Lower near
		int lower_near_texel_x = lower_cascade_near_probe_column_texel_x + lower_direction_id;	// Does not need clamping.
		ivec2 physical_lower_near_texel_position = logical_to_physical_lower_cascade_ray_texel_position(ivec2(lower_near_texel_x, output_texel_position.y));
		radiance = texelFetch(shorter_rays, ivec3(physical_lower_near_texel_position, 0), 0);
		transmittance = texelFetch(shorter_rays, ivec3(physical_lower_near_texel_position, 1), 0);
	
		int upper_direction_id = (direction_id + 1) >> 1;

		// Upper near
		int upper_near_texel_x = lower_cascade_near_probe_column_texel_x + upper_direction_id;	// Does not need clamping.
		ivec2 physical_upper_near_texel_position = logical_to_physical_lower_cascade_ray_texel_position(ivec2(upper_near_texel_x, output_texel_position.y));
		radiance += texelFetch(shorter_rays, ivec3(physical_upper_near_texel_position, 0), 0);
		vec4 upper_transmittance = texelFetch(shorter_rays, ivec3(physical_upper_near_texel_position, 1), 0);

		int lower_cascade_far_probe_column_texel_x = 
			lower_cascade_near_probe_column_texel_x + ray_casting_data.lower_cascade_rays_per_probe;
		int clamped_lower_cascade_far_probe_column_texel_x = 
			min(lower_cascade_far_probe_column_texel_x, ray_casting_data.lower_cascade_max_probe_column_texel_x);
		float lower_cascade_far_probe_column_is_inside_bounds = float
		(
			lower_cascade_far_probe_column_texel_x <= ray_casting_data.lower_cascade_max_probe_column_texel_x
		);
		int far_base_y = output_texel_position.y - ray_casting_data.lower_cascade_power_of_two;

		// Lower far
		int clamped_lower_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + upper_direction_id;
		int lower_far_texel_y = far_base_y + (lower_direction_id << 1);
		int clamped_lower_far_texel_y = clamp(lower_far_texel_y, 0, ray_casting_data.lower_cascade_max_probe_row);
		ivec2 physical_lower_far_texel_position = logical_to_physical_lower_cascade_ray_texel_position(ivec2(clamped_lower_far_texel_x, clamped_lower_far_texel_y));
		radiance += transmittance * 
			(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, ivec3(physical_lower_far_texel_position, 0), 0));
		transmittance *= mix
		(
			vec4(1.0),
			texelFetch(shorter_rays, ivec3(physical_lower_far_texel_position, 1), 0),
			lower_cascade_far_probe_column_is_inside_bounds
		);

		// Upper far
		int clamped_upper_far_texel_x = clamped_lower_cascade_far_probe_column_texel_x + lower_direction_id;
		int upper_far_texel_y = far_base_y + (upper_direction_id << 1);
		int clamped_upper_far_texel_y = clamp(upper_far_texel_y, 0, ray_casting_data.lower_cascade_max_probe_row);
		ivec2 physical_upper_far_texel_position = logical_to_physical_lower_cascade_ray_texel_position(ivec2(clamped_upper_far_texel_x, clamped_upper_far_texel_y));
		radiance += upper_transmittance * 
			(lower_cascade_far_probe_column_is_inside_bounds * texelFetch(shorter_rays, ivec3(physical_upper_far_texel_position, 0), 0));
		upper_transmittance *= mix
		(
			vec4(1.0),
			texelFetch(shorter_rays, ivec3(physical_upper_far_texel_position, 1), 0),
			lower_cascade_far_probe_column_is_inside_bounds
		);

		transmittance += upper_transmittance;

		radiance *= 0.5;
		transmittance *= 0.5;

	#elif RAY_TEXTURE_MODE == COLUMN_RAY_TEXTURE_MODE

		/*if (ray_casting_data.lower_cascade_max_ray_probe_column == 2)
		{
			radiance = vec4(0.0, 1.0, 0.0, 1.0);
			transmittance = vec4(1.0);
			return;
		}*/

		/*int ray_casting_data_rays_per_probe = ray_casting_data.rays_per_probe; //merged_to_cascade_power_of_two + 1;
		int ray_casting_data_skipped_rays_below_column = (ray_casting_data_rays_per_probe + 1) >> 1;
		int ray_casting_data_lower_cascade_rays_per_probe = ray_casting_data.lower_cascade_rays_per_probe; //cascade_merging_from_power_of_two + 1;
		int ray_casting_data_lower_cascade_skipped_rays_below_column = (ray_casting_data_lower_cascade_rays_per_probe + 1) >> 1;
		int ray_casting_data_lower_cascade_max_ray_probe_column = (edge_width - 1) / cascade_merging_from_power_of_two - 1;
		int ray_casting_data_lower_cascade_max_ray_probe_row = int(probe_grid_size.y) * ray_casting_data_lower_cascade_rays_per_probe - (ray_casting_data_lower_cascade_skipped_rays_below_column << 1) - 1;
		int ray_casting_data_g = ray_casting_data_lower_cascade_rays_per_probe << 1; //ray_casting_data_rays_per_probe << 1;
		int ray_casting_data_f = ray_casting_data_lower_cascade_rays_per_probe << cascade_merging_from; //ray_casting_data_rays_per_probe << merged_to_cascade;*/

		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		int ray_id_in_column = ray_casting_data.skipped_rays_below_column + output_texel_position.y;
		int probe_y = ray_id_in_column / ray_casting_data.rays_per_probe;
		int direction_id = ray_id_in_column - probe_y * ray_casting_data.rays_per_probe;

		// Some temporaries
		int a = (output_texel_position.x << 1) | 1;	// Does not need clamping

		int b = ray_casting_data.lower_cascade_rays_per_probe * probe_y - ray_casting_data.lower_cascade_skipped_rays_below_column;
		int d = direction_id >> 1;
		int h = b + d;

		int clamped_h = max(0, h);
		float h_is_inside = float(0 <= h);

		// IMPORTANT TODO: Use 16 bit floats for radiance and transmittance to reduce from 8 to 4 texel fetches.

		// Lower near
		radiance = h_is_inside * texelFetch(shorter_rays, ivec3(a, clamped_h, 0), 0);
		transmittance = mix(vec4(1.0), texelFetch(shorter_rays, ivec3(a, clamped_h, 1), 0), h_is_inside);
	
		// And some more temporaries needed from here
		int e = (direction_id + 1) >> 1;
		int i = b + e;
		int c = a + 1;

		int clamped_c = min(c, ray_casting_data.lower_cascade_max_ray_probe_column);
		float c_is_inside = float(c <= ray_casting_data.lower_cascade_max_ray_probe_column);

		// Lower far
		int lower_far_sample_y = i + d * ray_casting_data.g - ray_casting_data.f;
		int clamped_lower_far_sample_y = clamp(lower_far_sample_y, 0, ray_casting_data.lower_cascade_max_ray_probe_row);
		float lower_far_sample_y_is_inside = float(0 <= lower_far_sample_y) * float(lower_far_sample_y <= ray_casting_data.lower_cascade_max_ray_probe_row);
		float lower_far_sample_is_inside = c_is_inside * lower_far_sample_y_is_inside;
		radiance += transmittance * (lower_far_sample_is_inside * texelFetch(shorter_rays, ivec3(clamped_c, clamped_lower_far_sample_y, 0), 0));
		transmittance *= mix(vec4(1.0), texelFetch(shorter_rays, ivec3(clamped_c, clamped_lower_far_sample_y, 1), 0), lower_far_sample_is_inside);

		int clamped_i = min(i, ray_casting_data.lower_cascade_max_ray_probe_row);
		float i_is_inside = float(i <= ray_casting_data.lower_cascade_max_ray_probe_row);

		// Upper near
		radiance += i_is_inside * texelFetch(shorter_rays, ivec3(a, clamped_i, 0), 0);
		vec4 upper_transmittance = mix(vec4(1.0), texelFetch(shorter_rays, ivec3(a, clamped_i, 1), 0), i_is_inside);
	
		// Upper far
		int upper_far_sample_y = h + e * ray_casting_data.g - ray_casting_data.f;	// Does not need to be clamped
		int clamped_upper_far_sample_y = clamp(upper_far_sample_y, 0, ray_casting_data.lower_cascade_max_ray_probe_row);
		float upper_far_sample_y_is_inside = float(0 <= upper_far_sample_y) * float(upper_far_sample_y <= ray_casting_data.lower_cascade_max_ray_probe_row);
		float upper_far_sample_is_inside = c_is_inside * upper_far_sample_y_is_inside;
		radiance += upper_transmittance * (upper_far_sample_is_inside * texelFetch(shorter_rays, ivec3(clamped_c, clamped_upper_far_sample_y, 0), 0));
		upper_transmittance *= mix(vec4(1.0), texelFetch(shorter_rays, ivec3(clamped_c, clamped_upper_far_sample_y, 1), 0), upper_far_sample_is_inside);
	
		transmittance += upper_transmittance;

		radiance *= 0.5;
		transmittance *= 0.5;

		///////////////////////////////////////////////////////////////////////////////////////
		#if 1 == 0
			radiance = texelFetch(shorter_rays, ivec3(a, clamped_h, 0), 0);
			transmittance = vec4(1.0, 1.0, 1.0, 1.0);
			//radiance = vec4(1.0, 1.0, 1.0, 1.0);
		#endif

	#endif
}
