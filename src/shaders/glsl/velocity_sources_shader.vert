layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 vel;
layout(location = 2) in float weight;

out vec2 v_vel;
out float v_weight;

uniform mat3 u_matrix;

void main()
{
    v_vel = vel;
    v_weight = weight;
    gl_Position.xyw = u_matrix*vec3(pos, 1.0);
}
