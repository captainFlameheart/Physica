in vec4 cone_color;

layout (location = 0) out vec4 color;

void main()
{
	// TODO: Discard if outside cone
	color = cone_color;
}
