uniform sampler2D u_tex_density_sources;
uniform sampler2D u_tex_density_buffer;

uniform float u_dt;
uniform int   u_grid_res;

in vec2 v_tex;

out float frag_col;

void main()
{
    const ivec3 o = ivec3(-1,0,1);

    const float fractional_gain = 6.0; // Density is reduced to 1/6th due to diffusion
    const float scale = 1000.0;
    float f1 = scale * u_dt * u_grid_res;
    float f0 = fractional_gain * f1;
    
    float r = 1.0/(f1*5.0+1.0);

    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    frag_col = (f0 * float(texelFetch(u_tex_density_sources, ivec2(gl_FragCoord.xy), 0)) +
                f1 * float(texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).r) +
                f1 * float(texelFetch(u_tex_density_buffer, east, 0).r) +
                f1 * float(texelFetch(u_tex_density_buffer, west, 0).r) +
                f1 * float(texelFetch(u_tex_density_buffer, south, 0).r) +
                f1 * float(texelFetch(u_tex_density_buffer, north, 0).r)
               )*r;
}
