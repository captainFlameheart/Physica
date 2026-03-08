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

const float density = 1.0 / (0.05 * pi);
const float pi_times_density = pi * density;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;
	offset = offsets[offset_index];

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint body_position_radius_base = fixed_data.rigid_body_circle_body_position_radius_base;

	uint body_position_radius_index = body_position_radius_base + index;

	uvec4 body_position_radius = uvec4_data.data[body_position_radius_index];
	vec3 position_radius = uintBitsToFloat(body_position_radius.yzw);
	radius = position_radius.z;

	uint rigid_body_write_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint rigid_body_write_position_flags_index = rigid_body_write_position_flags_base + body_position_radius.x;

	uvec4 rigid_body_position_flags = uvec4_data.data[rigid_body_write_position_flags_index];

	offset *= radius;
	vec2 vertex = offset + position_radius.xy;
	
	// TODO: Handle large angle precision.
	float angle = float(int(rigid_body_position_flags.z)) * angle_unit_in_radians;
	float angle_cos = cos(angle);
	float angle_sin = sin(angle);
	vertex = mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * vertex;

	vec4 camera_offset = vec4(ivec2(rigid_body_position_flags.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += vertex;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
