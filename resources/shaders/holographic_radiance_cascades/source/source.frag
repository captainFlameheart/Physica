uniform sampler2DArray source;
uniform float layer;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(source, vec3(texture_position, layer));
}
