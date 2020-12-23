uniform sampler2D u_tex_velocity_buffer;
uniform sampler2D u_tex_velocity_divergence_buffer;

in vec2 v_tex;
out vec2 v_vel;

void main()
{
    const ivec3 o = ivec3(-1,0,1);
    ivec2 i_west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 i_north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 i_south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 i_east  = ivec2(gl_FragCoord.xy)+o.zy;

    float east = texelFetch(u_tex_velocity_divergence_buffer, i_east, 0).x;
    float west = texelFetch(u_tex_velocity_divergence_buffer, i_west, 0).x;
    float south = texelFetch(u_tex_velocity_divergence_buffer, i_south, 0).x;
    float north = texelFetch(u_tex_velocity_divergence_buffer, i_north, 0).x;
    vec2 vel_buffer = texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy), 0).xy;

    // v_vel = vel_buffer;
    v_vel = vel_buffer - vec2((east - west), (north - south));
    // v_vel = vec2((west - east), (north - south));
}
