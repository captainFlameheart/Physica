noperspective in vec2 offset;
noperspective in vec4 base_color;

layout (location = 1) out vec4 color;

void main()
{
    float weight = max(0.0, 1.0 - length(offset) / (RADIUS));
    color = vec4(base_color.rgb, base_color.a * weight * weight);
}
