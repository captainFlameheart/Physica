const uint offset_index_to_endpoint_index[6u] = uint[6u]
(
	0,
	1,
	1,

	1,
	0,
	0
);

const float offset_index_to_orthogonal_direction_sign[6u] = float[6u]
(
	1.0,
	1.0,
	-1.0,

	-1.0,
	-1.0,
	1.0
);

#define CULL_DEAD 0
#define CULL_ALIVE 0

#if CULL_DEAD == 1 || CULL_ALIVE == 1
	out float gl_CullDistance[1u];
#endif

out vec4 color;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_base = fixed_data.specific_contact_detector_indices_bases[rigid_body_triangle_contact_type];

	uint indices_index = indices_base + index;

	uvec2 indices = uvec2_data.data[indices_index];

	uint triangle_body_material_vertex_0_base = fixed_data.rigid_body_triangle_body_material_vertex_0_base;
	uint triangle_vertex_1_2_base = fixed_data.rigid_body_triangle_vertex_1_2_base;

	uint triangle_body_material_vertex_0_index_0 = triangle_body_material_vertex_0_base + indices.x;
	uint triangle_body_material_vertex_0_index_1 = triangle_body_material_vertex_0_base + indices.y;

	uint triangle_vertex_1_2_index_0 = triangle_vertex_1_2_base + indices.x;
	uint triangle_vertex_1_2_index_1 = triangle_vertex_1_2_base + indices.y;

	uvec4 triangle_body_material_vertex_0_0 = uvec4_data.data[triangle_body_material_vertex_0_index_0];
	uvec4 triangle_body_material_vertex_0_1 = uvec4_data.data[triangle_body_material_vertex_0_index_1];

	vec4 triangle_vertex_1_2_0 = vec4_data.data[triangle_vertex_1_2_index_0];
	vec4 triangle_vertex_1_2_1 = vec4_data.data[triangle_vertex_1_2_index_1];

	vec2 vertices_0[3u] =
	{
		uintBitsToFloat(triangle_body_material_vertex_0_0.zw),
		triangle_vertex_1_2_0.xy,
		triangle_vertex_1_2_0.zw,
	};
	vec2 vertices_1[3u] =
	{
		uintBitsToFloat(triangle_body_material_vertex_0_1.zw),
		triangle_vertex_1_2_1.xy,
		triangle_vertex_1_2_1.zw,
	};

	uint rigid_body_write_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint rigid_body_write_position_flags_index_0 = rigid_body_write_position_flags_base + triangle_body_material_vertex_0_0.x;
	uint rigid_body_write_position_flags_index_1 = rigid_body_write_position_flags_base + triangle_body_material_vertex_0_1.x;

	uvec4 rigid_body_position_flags_0 = uvec4_data.data[rigid_body_write_position_flags_index_0];
	uvec4 rigid_body_position_flags_1 = uvec4_data.data[rigid_body_write_position_flags_index_1];

	uvec4 rigid_body_position_flags[2u] = uvec4[2u]
	(
		rigid_body_position_flags_0,
		rigid_body_position_flags_1
	);

	vec2 direction = normalize(vec2(ivec2(rigid_body_position_flags_0.xy - rigid_body_position_flags_1.xy)));
	vec2 orthogonal_direction = vec2(-direction.y, direction.x);

	uint endpoint_index = offset_index_to_endpoint_index[offset_index];
	uvec4 endpoint_rigid_body_position_flags = rigid_body_position_flags[endpoint_index];

	float direction_sign = float(int(1u - 2u * endpoint_index));
	float orthogonal_direction_sign = offset_index_to_orthogonal_direction_sign[offset_index];

	vec4 camera_offset = vec4(ivec2(endpoint_rigid_body_position_flags.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;

	// TODO: Handle large angle precision.
	float endpoint_rigid_body_angle = float(rigid_body_position_flags[endpoint_index]) * angle_unit_in_radians;

	float angle_cos = cos(endpoint_rigid_body_angle);
	float angle_sin = sin(endpoint_rigid_body_angle);

	const float weight = 1.0 / 3.0;
	vec2 triangle_positions[2u] = vec2[2u]
	(
		weight * (vertices_0[0u] + vertices_0[1u] + vertices_0[2u]),
		weight * (vertices_1[0u] + vertices_1[1u] + vertices_1[2u])
	);

	vec2 endpoint_triangle_position = triangle_positions[endpoint_index];
	
	camera_offset.xy += mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * endpoint_triangle_position;

	camera_offset.xy += (0.0 * direction_sign) * direction + (0.08 * orthogonal_direction_sign) * orthogonal_direction;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;

	color = vec4(1.0, 1.0, 1.0, default_reflectivity);

	#if CULL_DEAD
		gl_CullDistance[0u] = 0.0;
	#endif
	#if CULL_ALIVE
		gl_CullDistance[0u] = -1.0;
	#endif
}
