/* Expected to be concatenated from CPU:



*/

layout(shared, binding = FLUENCE_GATHERING_BINDING) uniform Fluence_Gathering_Data
{
	int direction_mask;
	int cascade;
	int rays_per_probe;
	int skipped_rays_below_column;
	int upper_cascade_probe_column_texel_x_mask;
	int upper_cascade;
} fluence_gathering_data;

uniform sampler2DArray rays;
uniform sampler2D upper_cascade_fluence;

layout (location = 0) out vec4 fluence;

void main()
{
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int direction_id = output_texel_position.x & fluence_gathering_data.direction_mask;

	int probe_column = output_texel_position.x >> fluence_gathering_data.cascade;
	
	int lower_near_ray_sample_y = (
		output_texel_position.y * fluence_gathering_data.rays_per_probe - fluence_gathering_data.skipped_rays_below_column 
		+ direction_id
	);

	float half_angle;	// TODO

	// Lower near ray
	fluence = half_angle * texelFetch(rays, ivec3(probe_column, lower_near_ray_sample_y, 0), 0);
	vec4 lower_near_transmittance = texelFetch(rays, ivec3(probe_column, lower_near_ray_sample_y, 1), 0);
	vec4 shifted_lower_near_transmittance = lower_near_transmittance - 1.0;

	int interpolating = probe_column & 1;

	int far_ray_sample_x = probe_column + interpolating;
	int lower_far_ray_sample_y = lower_near_ray_sample_y - interpolating * fluence_gathering_data.rays_per_probe;

	// Lower far ray
	vec4 lower_far_fluence = half_angle * texelFetch(rays, ivec3(far_ray_sample_x, lower_far_ray_sample_y, 0), 0);
	vec4 lower_far_transmittance = texelFetch(rays, ivec3(far_ray_sample_x, lower_far_ray_sample_y, 1), 0);

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);
	float lower_near_transmit_factor = 1.0 + not_interpolating_float * shifted_lower_near_transmittance;

	int near_sample_probe_column_texel_x = output_texel_position.x & fluence_gathering_data.upper_cascade_probe_column_texel_x_mask;
	int lower_sample_direction_id = direction_id << 1;
	int lower_near_fluence_sample_x = near_sample_probe_column_texel_x + lower_sample_direction_id;
	int lower_near_fluence_sample_y = output_texel_position.y - not_interpolating;

	// Lower near fluence
	fluence += lower_near_transmit_factor * texelFetch(upper_cascade_fluence, ivec2(lower_near_fluence_sample_x, lower_near_fluence_sample_y), 0);

	int far_y_offset = interpolating + 1;

	int lower_far_fluence_sample_x = lower_near_fluence_sample_x + (interpolating << fluence_gathering_data.upper_cascade);
	int lower_far_fluence_sample_y = output_texel_position.y - far_y_offset;

	// Lower far fluence
	lower_far_fluence += lower_far_transmittance * texelFetch(upper_cascade_fluence, ivec2(lower_far_fluence_sample_x, lower_far_fluence_sample_y), 0);

	float interpolating_float = float(interpolating);
	float lower_far_transmit_factor = 1.0 + interpolating_float * shifted_lower_near_transmittance;
	fluence += lower_far_transmit_factor * lower_far_fluence;

	int upper_ray_sample_y = lower_ray_sample_y + 1;

	fluence *= 0.5;
}
