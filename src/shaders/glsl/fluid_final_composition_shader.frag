uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_density_buffer;
uniform sampler2D u_tex_ground_distorted;
uniform sampler2D u_tex_velocities;

out vec3 frag_col;

void main()
{
    float g_b = texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy), 0).x;
    float g_d = texelFetch(u_tex_ground_distorted, ivec2(gl_FragCoord.xy), 0).x;
    float v = length(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy),0));
    vec3 d = texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).rgb;
    frag_col.r = 0.05*g_b + 0.1*g_d  + 0.025 * clamp(0.1*v, 0.0, 2.0) + 0.2*clamp(0.05*d.r, 0.0, 10.0);
    frag_col.g = 0.05*g_b + 0.1*g_d  + 0.025 * clamp(0.1*v, 0.0, 2.0) + 0.2*clamp(0.05*d.g, 0.0, 10.0);
    frag_col.b =  0.3*g_b + 0.6*g_d  + 0.025 * clamp(0.1*v, 0.0, 2.0) + 0.2*clamp(0.05*d.b, 0.0, 10.0);
}
