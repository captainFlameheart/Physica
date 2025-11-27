uniform sampler2DArray source;
uniform float layer;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(source, vec3(texture_position, layer));
//	color = texelFetch(source, ivec3(ivec2(gl_FragCoord.xy), 0), 0);//texture(source, vec3(gl_FragCoord.xy / textureSize(source).xy, 0.0));
	//color = texture(source, vec3(0.0, 0.0, 0.0));
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}
