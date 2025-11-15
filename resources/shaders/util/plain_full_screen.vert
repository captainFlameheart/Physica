const vec2 vertices[6u] = vec2[6u]
(
	vec2(1.0, 1.0), 
	vec2(-1.0, 1.0), 
	vec2(-1.0, -1.0), 

	vec2(-1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, 1.0)
);

void main()
{
	vec2 vertex = vertices[gl_VertexID];
	gl_Position = vec4(vertex, 0.0, 1.0);
}
