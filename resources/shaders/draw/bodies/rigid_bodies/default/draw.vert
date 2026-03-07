const float thickness = 0.2;

const vec2 offsets[12u] = vec2[12u]
(
	vec2(1.0, thickness),
	vec2(-1.0, thickness),
	vec2(-1.0, -thickness),

	vec2(-1.0, -thickness),
	vec2(1.0, -thickness),
	vec2(1.0, thickness),

	vec2(thickness, 1.0),
	vec2(-thickness, 1.0),
	vec2(-thickness, -1.0),

	vec2(-thickness, -1.0),
	vec2(thickness, -1.0),
	vec2(thickness, 1.0)
);

const float density = 1.0 / (0.05 * pi);
const float pi_times_density = pi * density;

void main()
{
	uint index = gl_VertexID / 12u;
	uint offset_index = gl_VertexID - index * 12u;
	vec2 offset = offsets[offset_index];

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint inverse_mass_inertia_base = fixed_data.rigid_body_inverse_mass_inertia_base;
	uint write_position_turn_base = fixed_data.rigid_body_write_position_turn_base;

	uint inverse_mass_inertia_index = inverse_mass_inertia_base + index;
	uint write_position_turn_index = write_position_turn_base + index;

	vec2 inverse_mass_inertia = vec2_data.data[inverse_mass_inertia_index];
	uvec4 position_turn = uvec4_data.data[write_position_turn_index];

	float has_infinite_mass = float(inverse_mass_inertia.x == 0.0);
	inverse_mass_inertia.x = mix(inverse_mass_inertia.x, 1.0, has_infinite_mass);
	float radius = 1.0 / (inverse_mass_inertia.x * pi_times_density);
	offset *= radius;
	// TODO: Handle large angle precision.
	float angle = float(int(position_turn.z)) * angle_unit_in_radians;
	float angle_cos = cos(angle);
	float angle_sin = sin(angle);
	offset = mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * offset;

	vec4 camera_offset = vec4(ivec2(position_turn.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += offset;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
