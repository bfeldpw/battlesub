uniform sampler2D u_tex_buffer;
uniform sampler2D u_tex_sources;

in vec2 v_tex;

out vec2 frag_value;

void main()
{
    vec2 v_src = texelFetch(u_tex_sources, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 v_buf = texelFetch(u_tex_buffer, ivec2(gl_FragCoord.xy), 0).xy;

    vec2 v_buf_unit = normalize(v_buf+vec2(-0.1, 0.1));
    vec2 v_prj = dot(v_src, v_buf_unit)*v_buf_unit;

    frag_value = v_buf.xy+(v_src.xy-v_prj.xy)*0.9999;
}
