uniform sampler2D fluid_texture;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

void main()
{
    vec4 texture_color = texture(fluid_texture, texture_position);
    if (texture_color.a < 1.0)
    {
        color.rgb = texture_color.rgb * texture_color.a * 0.3f;
    }
    else
    {
        color.rgb = vec3(texture_color.rgb / texture_color.a);
        if (texture_color.a < 1.3)
        {
            color.rgb *= 0.5;
        }
    }
    color.a = 1.0;
}
