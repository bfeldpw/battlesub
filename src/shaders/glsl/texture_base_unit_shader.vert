uniform float u_tex_scale_x;
uniform float u_tex_scale_y;

out vec2 v_tex;

const vec2 pos[3] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

const vec2 tex[3] = vec2[]
(
    vec2(0.0, 0.0),
    vec2(2.0, 0.0),
    vec2(0.0, 2.0)
);

void main()
{
    v_tex = vec2(u_tex_scale_x*tex[gl_VertexID].x,
                 u_tex_scale_y*tex[gl_VertexID].y);
    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
}
