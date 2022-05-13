uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_density_buffer;
uniform sampler2D u_tex_ground_distorted;
uniform sampler2D u_tex_velocities;

out float frag_col;

void main()
{
    float g_b = texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy), 0).x;
    float g_d = texelFetch(u_tex_ground_distorted, ivec2(gl_FragCoord.xy), 0).x;
    float v = length(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy),0));
    float d = texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).x;
    frag_col =  0.25*g_b + 0.25*g_d  + 0.1 * clamp(0.1*v, 0.0, 1.0) + 0.3*clamp(0.05*d, 0.0, 2.0);
}
