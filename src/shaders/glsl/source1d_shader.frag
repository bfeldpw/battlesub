uniform sampler2D u_tex_buffer;
uniform sampler2D u_tex_sources;

in vec2 v_tex;

out vec3 frag_value;

void main()
{
    frag_value = (texelFetch(u_tex_buffer, ivec2(gl_FragCoord.xy), 0).rgb +
                  texelFetch(u_tex_sources, ivec2(gl_FragCoord.xy), 0).rgb)*0.99;
}
