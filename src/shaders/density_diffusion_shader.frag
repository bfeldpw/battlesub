uniform sampler2D u_tex_density_sources;
uniform sampler2D u_tex_density_buffer;

uniform float u_alpha;
uniform float u_diffusion_rate;
uniform float u_dissipation;

in vec2 v_tex;

out float frag_col;

void main()
{
    const ivec3 o = ivec3(-1,0,1);

    float r_beta = 1.0/(u_alpha+u_diffusion_rate*(4.0+u_dissipation));

    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    frag_col = (u_alpha          * float(texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).r) +
                                   float(texelFetch(u_tex_density_sources, ivec2(gl_FragCoord.xy), 0)) +
                u_diffusion_rate * float(texelFetch(u_tex_density_buffer, east, 0).r) +
                u_diffusion_rate * float(texelFetch(u_tex_density_buffer, west, 0).r) +
                u_diffusion_rate * float(texelFetch(u_tex_density_buffer, south, 0).r) +
                u_diffusion_rate * float(texelFetch(u_tex_density_buffer, north, 0).r)
               )*r_beta;
}
