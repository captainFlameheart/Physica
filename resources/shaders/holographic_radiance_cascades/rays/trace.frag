// Expected to be concatenated from CPU:
// const uint cascade = ?u;
// const uint step_count = ?u;

// IMPORTANT TODO: Put into uniform ring buffer
uniform float step_world_distance = ?;

uniform sampler2DArray source;

layout (location = 0) out vec4 radiance;
layout (location = 1) out vec4 transmittance;

void main()
{
	radiance = vec4(0.0);
	transmittance = vec4(1.0);
	
	vec2 sample_point = vec2(0.0, 0.0);
	for (uint i = 0u; i < step_count; ++i)
	{
		// TODO: Maybe change order of emission and absorption.
		vec4 emission = texture(source, vec3(sample_point, 1.0));
		vec4 absorption = texture(source, vec3(sample_point, 2.0));
		
		vec4 scaled_attenuation = absorption * step_world_distance;
		vec4 transmittance_factor = exp(-scaled_attenuation)

		vec4 small_emission_factor = step_world_distance * (1.0 - 0.5 * scaled_attenuation); // Second order taylor approximation for small attenuation.
		vec4 large_emission_factor = emission * (1.0 - transmittance_factor) / absorption;
		vec4 mix_factor = step(1e-4, abs(absorption)); // TODO: abs is unnecessary if we disallow negative absorptions.
		vec4 emission_factor = mix(small_emission_factor, large_emission_factor, mix_factor);

		radiance += (emission * transmittance) * emission_factor;
		transmittance *= transmittance_factor;

		sample_point += 
	}
	
	radiance = texture(source, vec3(texture_position, layer));
}
