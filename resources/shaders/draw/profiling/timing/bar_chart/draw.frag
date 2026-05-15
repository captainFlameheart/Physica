flat in vec4 timestamp_color;

layout(location = 0) out vec4 color;

void main()
{
	color = timestamp_color;
}
