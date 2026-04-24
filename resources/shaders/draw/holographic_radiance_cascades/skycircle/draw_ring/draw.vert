const vec2 vertices[6u] = vec2[6u]
(
	vec2(-1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, 1.0),

	vec2(1.0, 1.0), 
	vec2(-1.0, 1.0), 
	vec2(-1.0, -1.0)
);

flat out vec2 radii;
noperspective out vec2 offset;

void main()
{
	vec2 unit_z_frustum_half_size = vec2(2.0, 1.0);	// TODO: Make dynamic.
	vec2 projection = 1.0 / unit_z_frustum_half_size;
	radii.x = min(unit_z_frustum_half_size.x, unit_z_frustum_half_size.y);
	radii.y = radii.x - 0.1;

	offset = vertices[gl_VertexID];
	gl_Position = vec4(projection * offset, 0.0, 1.0);
}
