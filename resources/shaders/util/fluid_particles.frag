noperspective in vec2 offset;

layout (location = 1) out vec4 color;

void main()
{
    // TODO: Have these parameters depend on the type of fluid
    float R = 0.4;
    float G = 0.4;
    float B = 1.0;
    float A = 4.0;

    if ((gl_PrimitiveID / 2u) % 2u == 0u)
    {
        R = 1.0;
        G = 1.0;
        B = 0.0;
    }

    float weight = max(0.0, (1.0 - length(offset) / (RADIUS)));
    color = vec4(R, G, B, A * weight * weight);
}
