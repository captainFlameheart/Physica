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

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;

	uint position_velocity_base = fixed_data.point_mass_position_velocity_base;
	uint inverse_mass_base = fixed_data.point_mass_inverse_mass_base;

	uint position_velocity_index = position_velocity_base + index;
	uint inverse_mass_index = inverse_mass_base + index;

	uvec2 position = 

	ivec2 position = fluid_position.p[particle_index];
	ivec2 velocity = fluid_velocity.v[particle_index];

	uint offset_index = gl_VertexID % 6u;
	offset = offsets[offset_index];

	// Lava
	base_color = mix(vec4(1.0, 0.0, 0.0, 1.3), vec4(1.0, 1.0, 0.5, 1.2), min(1.0, 0.000004 * length(velocity)));

	// Water
	//base_color = mix(vec4(0.0, 0.0, 1.0, 1.3), vec4(0.0, 1.0, 1.0, 1.2), min(1.0, 0.000004 * length(velocity)));

	// Plasma
	//base_color = mix(vec4(0.8, 0.0, 0.8, 1.3), vec4(1.0, 0.5, 1.0, 1.2), min(1.0, 0.000004 * length(velocity)));

	/*if (particle_index < 3560u)
	{
		base_color = mix(vec4(1.0, 0.0, 0.0, 1.3), vec4(1.0, 1.0, 0.5, 1.2), min(1.0, 0.000004 * length(velocity)));
	}
	else
	{
		base_color = mix(vec4(1.0, 0.2, 0.0, 1.3), vec4(1.0, 1.0, 0.5, 1.2), min(1.0, 0.000004 * length(velocity)));
	}*/

	/*if (particle_index < 3560u)
	{
		base_color = mix(vec4(1.0, 0.0, 0.0, 1.3), vec4(1.0, 1.0, 0.5, 1.2), min(1.0, 0.000004 * length(velocity)));
	}
	else
	{
		base_color = mix(vec4(0.0, 0.0, 1.0, 1.3), vec4(0.0, 1.0, 1.0, 1.2), min(1.0, 0.000004 * length(velocity)));
	}*/

	vec2 camera_relative_xy = vec2(position - camera.xy) + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
