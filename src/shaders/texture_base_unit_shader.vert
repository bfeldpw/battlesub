layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec2 v_tex;

void main()
{
    v_tex = tex;
    gl_Position.xy = pos;
}
