layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 amount;

out vec3 v_amount;

uniform mat3 u_matrix;

void main()
{
    v_amount = amount;
    gl_Position.xyw = u_matrix*vec3(pos, 1.0);
}
