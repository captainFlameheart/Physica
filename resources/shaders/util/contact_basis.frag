layout (location = 0) out vec4 color;

void main()
{
	float local_line_index = float(gl_PrimitiveID % 2u);
	color = vec4(1.0 - local_line_index, local_line_index, 0.0, 1.0);
}
