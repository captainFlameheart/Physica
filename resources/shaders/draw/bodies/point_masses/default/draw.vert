const vec2 offsets[6u] = vec2[6u]
(
	vec2(1.0, 1.0), 
	vec2(-1.0, 1.0), 
	vec2(-1.0, -1.0), 

	vec2(-1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, 1.0)
);

out vec2 offset;
out float radius;

const float density = 1.0 / (0.02 * pi);
const float pi_times_density = pi * density;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;
	offset = offsets[offset_index];

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint position_velocity_base = fixed_data.point_mass_position_velocity_base;
	uint inverse_mass_base = fixed_data.point_mass_inverse_mass_base;

	uint position_velocity_index = position_velocity_base + index;
	uint inverse_mass_index = inverse_mass_base + index;

	uvec4 position_velocity = uvec4_data.data[position_velocity_index];
	float inverse_mass = float_data.data[inverse_mass_index];
	
	radius = 1.0 / (inverse_mass * pi_times_density);
	offset *= radius;

	vec4 camera_offset = vec4(vec2(ivec2(position_velocity.xy - camera_position.xy)) + offset, float(-int(camera_position.z)), float(1u));
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
