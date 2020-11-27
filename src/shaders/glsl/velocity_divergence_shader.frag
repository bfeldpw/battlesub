uniform sampler2D u_tex_velocity_buffer;

in vec2 v_tex;
out float div;

void main()
{
    const ivec3 o = ivec3(-1,0,1);
    ivec2 i_west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 i_north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 i_south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 i_east  = ivec2(gl_FragCoord.xy)+o.zy;

    vec2 east = texelFetch(u_tex_velocity_buffer, i_east, 0).xy;
    vec2 west = texelFetch(u_tex_velocity_buffer, i_west, 0).xy;
    vec2 south = texelFetch(u_tex_velocity_buffer, i_south, 0).xy;
    vec2 north = texelFetch(u_tex_velocity_buffer, i_north, 0).xy;

    div = ((west.x - east.x) + (north.y - south.y));
}
