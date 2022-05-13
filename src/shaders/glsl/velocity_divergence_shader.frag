uniform sampler2D u_tex_velocity_buffer;

in vec2 v_tex;
out float div;

void main()
{
    const ivec3 o = ivec3(-1,0,1);
    ivec2 i_w  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 i_n = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 i_s = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 i_e  = ivec2(gl_FragCoord.xy)+o.zy;

    float east = texelFetch(u_tex_velocity_buffer, i_e, 0).x;
    float west = texelFetch(u_tex_velocity_buffer, i_w, 0).x;
    float south = texelFetch(u_tex_velocity_buffer, i_s, 0).y;
    float north = texelFetch(u_tex_velocity_buffer, i_n, 0).y;

    div = -((east - west) + (north - south));
}
