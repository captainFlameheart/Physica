noperspective in vec2 offset;

layout (location = 1) out vec4 color;

void main()
{
    // TODO: Have these parameters depend on the type of fluid
    float R = 0.0;
    float G = 0.0;
    float B = 1.0;
    float A = 2.0;

    color = vec4(R, G, B, A * max(0.0, (1.0 - length(offset) / (RADIUS))));
}
