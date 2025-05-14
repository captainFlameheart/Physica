uniform sampler2D fluid_texture;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

void main()
{ 
    color = texture(fluid_texture, texture_position);
}
