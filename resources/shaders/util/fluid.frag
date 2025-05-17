uniform sampler2D fluid_texture;

noperspective in vec2 texture_position;

layout (location = 0) out vec4 color;

#define outline_threshold 2.0

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
        if (texture_color.a < outline_threshold)
        {
            color.rgb += 0.5; //mix(0.5 * color.rgb, color.rgb, texture_color.a / outline_threshold);
        }
    }
    color.a = 1.0;
}
