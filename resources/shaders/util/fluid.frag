uniform sampler2D fluid_texture;

noperspective in vec2 texture_position;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;


#define outline_threshold 2.0

void main()
{
    absorption = vec4(0.0, 0.0, 0.0, 0.0);

    vec4 texture_color = texture(fluid_texture, texture_position);
    if (texture_color.a < 1.0)
    {
        albedo.rgb = texture_color.rgb * texture_color.a * 0.1f;//0.3f;
    }
    else
    {
        albedo.rgb = vec3(texture_color.rgb / texture_color.a);
        absorption.rgb = vec3(0.1);
        if (texture_color.a < outline_threshold)
        {
            absorption.rgb = vec3(0.05);
            albedo.rgb += 0.5; //mix(0.5 * albedo.rgb, albedo.rgb, texture_color.a / outline_threshold);
        }
    }
    albedo.a = 1.0;
    emission = albedo * 0.5;
    scattering = vec4(0.0, 0.0, 0.0, 0.0);
}
