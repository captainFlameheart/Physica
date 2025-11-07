const vec4 cascade_colors[] = 
{
	vec4(0xF2, 0x85, 0x00, 1.0), 
	vec4(0xFF, 0x00, 0x00, 1.0), 
	vec4(0x00, 0xFF, 0x00, 1.0), 
	vec4(0x00, 0x00, 0xFF, 1.0), 
};

uniform uint cascade;

layout (location = 0) out vec4 color;

void main()
{
	color = cascade_colors[cascade % cascade_colors.length()];
}
