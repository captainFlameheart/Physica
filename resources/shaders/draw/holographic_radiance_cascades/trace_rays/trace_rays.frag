/* Expected to be concatenated from CPU:

#define EAST_DIRECTION ?
#define WEST_DIRECTION ?
#define NORTH_DIRECTION ?
#define SOUTH_DIRECTION ?

#define DIRECTION ?

const int rays_per_probe = ?;
const int cascade_power_of_two = ?;

const vec2 probe_grid_full_step_to_sample_step_factor = ?;	// TODO: Only store y component, see below.
const vec2 probe_grid_point_to_sample_point_bias = ?;
const vec2 probe_grid_point_to_sample_point_factor = ?;
const vec2 probe_grid_full_step_to_sample_step_projection = ?;

const uint step_count = ?u;

#define METER_INVERSE ?	// TODO: Remove

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

layout(location = source_image_texture_unit) uniform sampler2DArray source;

layout(location = rays_radiance_layer) out vec4 radiance;
layout(location = rays_transmittance_layer) out vec4 transmittance;

void main()
{
	radiance = vec4(0.0);
	transmittance = vec4(1.0);
	
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);

	int probe_column = X(output_texel_position) / rays_per_probe;
	int probe_column_texel_x = probe_column * rays_per_probe;
	int direction_id = X(output_texel_position) - probe_column_texel_x;

	// IMPORTANT TODO: float(cascade_power_of_two) * probe_grid_full_step_to_sample_step_factor.x should be precomputed by CPU.
	// (sample_step * 0.5).x can then also be precomputed. world_step_distance can also be optimized.
	vec2 probe_grid_full_step = VEC2(cascade_power_of_two, (direction_id << 1) - cascade_power_of_two);

	// MUST TODO: Update probe_grid_point_to_sample_point_factor and probe_grid_full_step_to_sample_step_factor

	vec2 sample_step = probe_grid_full_step * probe_grid_full_step_to_sample_step_factor;
	// TODO: Check that we have accounted for the negative sign before the bias on the CPU.
	vec2 sample_point = VEC2(probe_column + 1, Y(output_texel_position)) * probe_grid_point_to_sample_point_factor - probe_grid_point_to_sample_point_bias + sample_step * 0.5;

	// IMPORTANT TODO: We can do some of these operations on the CPU.
	#if DIRECTION == WEST_DIRECTION
		sample_step.x = -sample_step.x;
		sample_point.x = 1.0 - sample_point.x;
	#elif DIRECTION == SOUTH_DIRECTION
		sample_step.y = -sample_step.y;
		sample_point.y = 1.0 - sample_point.y;
	#endif

	ivec4 camera_position = ivec4(fixed_data.camera_position);
	float world_step_distance = length(probe_grid_full_step * probe_grid_full_step_to_sample_step_projection) * float(camera_position.z) * length_unit_in_meters;

	// VERY IMPORTANT TODO: The source texture should be considered to cover more than [0.0, 1.0] in s- and t-directions due to 
	// interpolation at the edges.

	// TODO: Allow camera to not look straight towards the world plane.
	for (uint i = 0u; i < step_count; ++i)
	{
		// TODO: Maybe change order of emission and absorption.
		vec4 emission = texture(source, vec3(sample_point, 1.0));
		vec4 absorption = texture(source, vec3(sample_point, 2.0));

		vec4 clamped_absorption = max(absorption, 1e-4);	// TODO: Maybe handle negative absorptions

		vec4 scaled_attenuation = absorption * world_step_distance;
		vec4 transmittance_factor = exp(-scaled_attenuation);

		vec4 small_emission_factor = world_step_distance * (1.0 - 0.5 * scaled_attenuation); // Second order taylor approximation for small attenuation.
		// TODO: Verify that the emission should indeed be removed from the large_emission_factor
		vec4 large_emission_factor = /*emission * */ (1.0 - transmittance_factor) / clamped_absorption;
		vec4 mix_factor = step(1e-4, absorption);
		vec4 emission_factor = mix(small_emission_factor, large_emission_factor, mix_factor);

		radiance += (emission * transmittance) * emission_factor;
		transmittance *= transmittance_factor;

		// TODO: Verify that loop unrolling occurs and that this increment is removed for the last iteration.
		sample_point += sample_step;
	}
}
