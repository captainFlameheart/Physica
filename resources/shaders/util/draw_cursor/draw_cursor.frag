/* Expected to be concatenated from the CPU:



*/

uniform float radius;

uniform vec4 main_albedo;
uniform vec4 main_emission;
uniform vec4 main_attenuation;
uniform vec4 main_scattering;

noperspective in vec2 offset;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 attenuation;
layout(location = 3) out vec4 scattering;

void main()
{
    float distance_squared = dot(offset, offset);
    if (distance_squared > radius * radius)
    {
        discard;
    }

    albedo = main_albedo;
    emission = main_emission;
    attenuation = main_attenuation;
    scattering = main_scattering;
}
