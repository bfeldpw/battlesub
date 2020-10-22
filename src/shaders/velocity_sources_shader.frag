in vec2 v_vel;
in float v_weight;

out vec3 fragmentColor;

void main()
{
    fragmentColor.xy = v_vel;
    fragmentColor.z = v_weight;
}
