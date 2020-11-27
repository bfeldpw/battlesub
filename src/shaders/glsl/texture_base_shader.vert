layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec2 v_tex;

uniform mat3 u_matrix;

void main()
{
    v_tex = tex;
    gl_Position.xyw = u_matrix*vec3(pos, 1.0);
}
