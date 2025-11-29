/* Expected to be concatenated from CPU:

const int skipped_rays_below_column = ?;
const int rays_per_probe = ?;
const int cascade_power_of_two = ?;

const vec2 probe_grid_full_step_to_sample_step_factor = ?;	// TODO: Only store y component, see below
const vec2 probe_grid_point_to_sample_point_factor = ?;
const vec2 probe_grid_full_step_to_sample_step_projection = ?;

const uint step_count = ?u;
*/

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

uniform sampler2DArray source;

layout (location = 0) out vec4 radiance;
layout (location = 1) out vec4 transmittance;

void main()
{
	// IMPORTANT TODO: Rays should be stored with directions expanded along the s-axis.

	radiance = vec4(0.0);
	transmittance = vec4(1.0);
	
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int ray_id_in_column = skipped_rays_below_column + output_texel_position.y;
	int probe_y = ray_id_in_column / rays_per_probe;
	int direction_id = ray_id_in_column - probe_y * rays_per_probe;
	
	// IMPORTANT TODO: float(cascade_power_of_two) * probe_grid_full_step_to_sample_step_factor.x should be precomputed by CPU.
	// (sample_step * 0.5).x can then also be precomputed. world_step_distance can also be optimized.
	vec2 probe_grid_full_step = vec2(cascade_power_of_two, (direction_id << 1) - cascade_power_of_two);

	vec2 sample_step = probe_grid_full_step * probe_grid_full_step_to_sample_step_factor;
	vec2 sample_point = vec2(output_texel_position.x + 1, probe_y) * probe_grid_point_to_sample_point_factor + sample_step * 0.5;
	
	float world_step_distance = length(probe_grid_full_step * probe_grid_full_step_to_sample_step_projection) * camera.z;

	// VERY IMPORTANT TODO: The source texture should be considered to cover more than [0.0, 1.0] in s- and t-directions due to 
	// interpolation at the edges.

	// TODO: Allow camera to not look straight towards the world plane.
	for (uint i = 0u; i < step_count; ++i)
	{
		// TODO: Maybe change order of emission and absorption.
		vec4 emission = texture(source, vec3(sample_point, 1.0));
		vec4 absorption = texture(source, vec3(sample_point, 2.0));
		
		vec4 scaled_attenuation = absorption * world_step_distance;
		vec4 transmittance_factor = exp(-scaled_attenuation);

		vec4 small_emission_factor = world_step_distance * (1.0 - 0.5 * scaled_attenuation); // Second order taylor approximation for small attenuation.
		vec4 large_emission_factor = emission * (1.0 - transmittance_factor) / absorption;
		vec4 mix_factor = step(1e-4, abs(absorption)); // TODO: abs is unnecessary if we disallow negative absorptions.
		vec4 emission_factor = mix(small_emission_factor, large_emission_factor, mix_factor);

		radiance += (emission * transmittance) * emission_factor;
		transmittance *= transmittance_factor;

		// TODO: Verify that loop unrolling occurs and that this increment is removed for the last iteration.
		sample_point += sample_step;
	}
}
