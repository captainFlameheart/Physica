/* Expected to be concatenated from CPU:



*/

layout(shared, binding = FLUENCE_GATHERING_BINDING) uniform Fluence_Gathering_Data
{
	int cascade;
	int rays_per_probe;
	int skipped_rays_below_column;
	int direction_mask;
} fluence_gathering_data;

uniform sampler2DArray rays;
uniform sampler2D higher_cascade_fluence;

layout (location = 0) out vec4 fluence;

void main()
{
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int direction_id = output_texel_position.x & fluence_gathering_data.direction_mask;

	int near_ray_sample_x = output_texel_position.x >> fluence_gathering_data.cascade;
	int lower_near_ray_sample_y = (
		output_texel_position.y * fluence_gathering_data.rays_per_probe - fluence_gathering_data.skipped_rays_below_column 
		+ direction_id
	);

	// Lower near ray
	texelFetch(rays, ivec3(near_ray_sample_x, lower_near_ray_sample_y, 0), 0);
	texelFetch(rays, ivec3(near_ray_sample_x, lower_near_ray_sample_y, 1), 0);

	int interpolating = near_ray_sample_x & 1;

	int far_ray_sample_x = near_ray_sample_x + interpolating;

	int lower_far_ray_sample_y = lower_near_ray_sample_y - interpolating * fluence_gathering_data.rays_per_probe;

	// Lower far ray
	texelFetch(rays, ivec3(far_ray_sample_x, lower_far_ray_sample_y, 0), 0);
	texelFetch(rays, ivec3(far_ray_sample_x, lower_far_ray_sample_y, 1), 0);

	int lower_near_fluence_sample_x = 

	int upper_ray_sample_y = lower_ray_sample_y + 1;
}
