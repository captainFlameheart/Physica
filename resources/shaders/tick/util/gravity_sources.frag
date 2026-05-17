noperspective in vec2 offset;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 emission;
layout (location = 2) out vec4 attenuation;
layout (location = 3) out vec4 scattering;


void main()
{
    float distance = length(offset);
    /*if (distance > GRAB_RADIUS + LIGHT_DISTANCE)
    {
        discard;
    }*/
    if (distance > GRAB_RADIUS)
    {
        discard;
        //float weight = (1.0 - (distance - (GRAB_RADIUS)) / (LIGHT_DISTANCE));
        //color = vec4(1.0, 0.0, 1.0, 0.2 * weight * weight);
    }
    else if (distance > GRAB_RADIUS - 0.1 * METER)
    {
        color = vec4(1.0, 0.5, 1.0, 0.2);
    }
    else
    {
        float distance_fraction = distance / (GRAB_RADIUS);
        color = vec4(distance_fraction, 0.0, distance_fraction, 0.2);
    }
    emission.rgb = vec3(2.0, 0.0, 4.0);
    emission.a = 0.0;
    attenuation = vec4(0.1);
    scattering = vec4(0.0);
}
