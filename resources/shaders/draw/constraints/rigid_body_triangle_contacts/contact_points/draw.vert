float gl_CullDistance[1u];

const float radius = 0.1 * meter_in_length_units;

const vec2 vertices[6u] =
{
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0),

	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),
};

out vec2 offset;

void main()
{
	uint contact_point_index = gl_VertexID / 6u;
	uint index = contact_point_index >> 1u;
	uint vertex_index = gl_VertexID - contact_point_index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_turns_base = fixed_data.rigid_body_triangle_contact_indices_turns_base;

	uint indices_turns_index = indices_turns_base + index;

	uvec4 indices_turns = uvec4_data.data[indices_turns_index];

	uint triangle_body_material_vertex_0_base = fixed_data.rigid_body_triangle_body_material_vertex_0_base;
	uint triangle_vertex_1_2_base = fixed_data.rigid_body_triangle_vertex_1_2_base;

	uint triangle_body_material_vertex_0_index[2u] =
	{
		triangle_body_material_vertex_0_base + indices_turns.x,
		triangle_body_material_vertex_0_base + indices_turns.y,
	};

	uint triangle_vertex_1_2_index[2u] =
	{
		triangle_vertex_1_2_base + indices_turns[0u],
		triangle_vertex_1_2_base + indices_turns[1u],
	};

	uvec4 triangle_body_material_vertex_0[2u] =
	{
		uvec4_data.data[triangle_body_material_vertex_0_index[0u]],
		uvec4_data.data[triangle_body_material_vertex_0_index[1u]],
	};

	vec4 triangle_vertex_1_2[2u] =
	{
		vec4_data.data[triangle_vertex_1_2_index[0u]],
		vec4_data.data[triangle_vertex_1_2_index[1u]],
	};

	vec2 triangle_vertices[2u][3u] =
	{
		vec2[3u](uintBitsToFloat(triangle_body_material_vertex_0[0u].zw), triangle_vertex_1_2[0u].xy, triangle_vertex_1_2[0u].zw),
		vec2[3u](uintBitsToFloat(triangle_body_material_vertex_0[1u].zw), triangle_vertex_1_2[1u].xy, triangle_vertex_1_2[1u].zw),
	};

	uint body_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint body_position_flags_index[2u] =
	{
		body_position_flags_base + triangle_body_material_vertex_0[0u].x,
		body_position_flags_base + triangle_body_material_vertex_0[1u].x,
	};

	ivec4 body_position_flags[2u] =
	{
		ivec4(uvec4_data.data[body_position_flags_index[0u]]),
		ivec4(uvec4_data.data[body_position_flags_index[1u]]),
	};

	vec2 body_position_difference = vec2(ivec2(body_position_flags[1u].xy) - ivec2(body_position_flags[0u].xy));

	// TODO: Handle large angle precision.
	float angles[2u] =
	{
		float(int(body_position_flags[0u].z)) * angle_unit_in_radians,
		float(int(body_position_flags[1u].z)) * angle_unit_in_radians,
	};
	vec2 rights[2u] =
	{
		vec2(cos(angles[0u]), sin(angles[0u])),
		vec2(cos(angles[1u]), sin(angles[1u])),
	};
	mat2 body_local_to_offset_transforms[2u] =
	{
		mat2(rights[0u], vec2(-rights[0u].y, rights[0u].x)) * meter_in_length_units,	// TODO: Store all lengths in length units.
		mat2(rights[1u], vec2(-rights[1u].y, rights[1u].x)) * meter_in_length_units,	// TODO: Store all lengths in length units.
	};

	vec2 triangle_vertex_offsets[2u][3u] =
	{
		vec2[3u](body_local_to_offset_transforms[0u] * triangle_vertices[0u][0u], body_local_to_offset_transforms[0u] * triangle_vertices[0u][1u], body_local_to_offset_transforms[0u] * triangle_vertices[0u][2u]),
		vec2[3u](body_local_to_offset_transforms[1u] * triangle_vertices[1u][0u], body_local_to_offset_transforms[1u] * triangle_vertices[1u][1u], body_local_to_offset_transforms[1u] * triangle_vertices[1u][2u]),
	};

	triangle_vertex_offsets[1u][0u] += body_position_difference;
	triangle_vertex_offsets[1u][1u] += body_position_difference;
	triangle_vertex_offsets[1u][2u] += body_position_difference;

	uint reference_edge = 0u;
	vec2 normal = triangle_vertex_offsets[0u][0u] - triangle_vertex_offsets[0u][1u];
	normal = normalize(vec2(-normal.y, normal.x));
	float normal_offset = dot(normal, triangle_vertex_offsets[0u][0u]);
	float smallest_penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[1u][0u]), min(dot(normal, triangle_vertex_offsets[1u][1u]), dot(normal, triangle_vertex_offsets[1u][2u])));
	
	normal = triangle_vertex_offsets[0u][1u] - triangle_vertex_offsets[0u][2u];
	normal = normalize(vec2(-normal.y, normal.x));
	normal_offset = dot(normal, triangle_vertex_offsets[0u][1u]);
	float penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[1u][0u]), min(dot(normal, triangle_vertex_offsets[1u][1u]), dot(normal, triangle_vertex_offsets[1u][2u])));
	if (penetration < smallest_penetration)
	{
		reference_edge = 1u;
		smallest_penetration = penetration;
	}

	normal = triangle_vertex_offsets[0u][2u] - triangle_vertex_offsets[0u][0u];
	normal = normalize(vec2(-normal.y, normal.x));
	normal_offset = dot(normal, triangle_vertex_offsets[0u][2u]);
	penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[1u][0u]), min(dot(normal, triangle_vertex_offsets[1u][1u]), dot(normal, triangle_vertex_offsets[1u][2u])));
	if (penetration < smallest_penetration)
	{
		reference_edge = 2u;
		smallest_penetration = penetration;
	}

	normal = triangle_vertex_offsets[1u][0u] - triangle_vertex_offsets[1u][1u];
	normal = normalize(vec2(-normal.y, normal.x));
	normal_offset = dot(normal, triangle_vertex_offsets[1u][0u]);
	penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[0u][0u]), min(dot(normal, triangle_vertex_offsets[0u][1u]), dot(normal, triangle_vertex_offsets[0u][2u])));
	if (penetration < smallest_penetration)
	{
		reference_edge = 3u;
		smallest_penetration = penetration;
	}

	normal = triangle_vertex_offsets[1u][1u] - triangle_vertex_offsets[1u][2u];
	normal = normalize(vec2(-normal.y, normal.x));
	normal_offset = dot(normal, triangle_vertex_offsets[1u][1u]);
	penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[0u][0u]), min(dot(normal, triangle_vertex_offsets[0u][1u]), dot(normal, triangle_vertex_offsets[0u][2u])));
	if (penetration < smallest_penetration)
	{
		reference_edge = 4u;
		smallest_penetration = penetration;
	}

	normal = triangle_vertex_offsets[1u][2u] - triangle_vertex_offsets[1u][0u];
	normal = normalize(vec2(-normal.y, normal.x));
	normal_offset = dot(normal, triangle_vertex_offsets[1u][2u]);
	penetration = normal_offset - min(dot(normal, triangle_vertex_offsets[0u][0u]), min(dot(normal, triangle_vertex_offsets[0u][1u]), dot(normal, triangle_vertex_offsets[0u][2u])));
	if (penetration < smallest_penetration)
	{
		reference_edge = 5u;
		smallest_penetration = penetration;
	}

	if (smallest_penetration < 0.0)
	{
		gl_CullDistance[0u] = -1.0;
		return;
	}

	uint reference_body = uint(reference_edge > 2u);
	uint incident_body = reference_body ^ 1u;

	reference_edge -= reference_body * 3u;

	vec2 reference_vertex_offsets[2u] =
	{
		triangle_vertex_offsets[reference_body][reference_edge],
		triangle_vertex_offsets[reference_body][uint(reference_edge < 2u) * (reference_edge + 1u)],
	};
	normal = reference_vertex_offsets[0u] - reference_vertex_offsets[1u];
	normal = normalize(vec2(-normal.y, normal.x));

	uint primary_incident_vertex_index = 0u;
	float min_incident_projection = dot(normal, triangle_vertex_offsets[incident_body][0u]);

	float incident_projection = dot(normal, triangle_vertex_offsets[incident_body][1u]);
	if (incident_projection < min_incident_projection)
	{
		primary_incident_vertex_index = 1u;
		min_incident_projection = incident_projection;
	}

	if (dot(normal, triangle_vertex_offsets[incident_body][2u]) < min_incident_projection)
	{
		primary_incident_vertex_index = 2u;
	}

	vec2 primary_incident_vertex_offset = triangle_vertex_offsets[incident_body][primary_incident_vertex_index];

	vec2 body_0_contact_point_offset = primary_incident_vertex_offset;

	/*vec2 offsets[2u] =
	{
		body_local_to_offset_transforms[0u] * (meter_in_length_units * vec2(0.0)),
		body_local_to_offset_transforms[1u] * (meter_in_length_units * vec2(0.0)),
	};

	float body_0_radius_fraction = 0.5;
	vec2 body_0_contact_point_offset = body_0_radius_fraction * (body_position_difference + (offsets[1u] - offsets[0u]));*/

	vec4 camera_offset = vec4(ivec2(body_position_flags[0u].xy - camera_position.xy), int(-camera_position.z), 1.0f);
	offset = vertices[vertex_index];
	camera_offset.xy += offset * radius;
	camera_offset.xy += body_0_contact_point_offset;
	camera_offset.xyz *= length_unit_in_meters;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;

	gl_CullDistance[0u] = 1.0;
}
