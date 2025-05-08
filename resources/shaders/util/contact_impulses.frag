layout (location = 0) out vec4 color;

void main()
{
	float direction_index = float(gl_PrimitiveID % 2u);
	color = vec4(1.0 - direction_index, direction_index, 0.0, 1.0);
}
