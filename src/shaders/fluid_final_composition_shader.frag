uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_density_buffer;
uniform sampler2D u_tex_ground_distorted;
uniform sampler2D u_tex_velocities;

out float frag_col;

void main()
{
    frag_col = 0.07 * texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy), 0).x +
               0.005 * texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).x +
               0.07 * texelFetch(u_tex_ground_distorted, ivec2(gl_FragCoord.xy), 0).x +
               0.00001 * length(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy),0));
}
