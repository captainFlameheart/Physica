noperspective in vec2 offset;

layout (location = 1) out vec4 color;

void main()
{
    float distance = length(offset);
    if (distance > RADIUS)
    {
        discard;
    }
    color = vec4(1.0, 0.0, 1.0, 1.0);
}
