uniform sampler2D fluid_texture;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

void main()
{
    color = texture(fluid_texture, texture_position);
    if (color.a < 1.0)
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        color = vec4(color.rgb / color.a, 1.0);
    }
}
