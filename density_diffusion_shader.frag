uniform sampler2D u_tex_density_sources;
uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_density_buffer;
uniform sampler2D u_tex_velocities;

uniform float u_dt;
uniform int   u_size;

in vec2 v_tex;

out float frag_col;

void main()
{
    const ivec3 o = ivec3(-1,0,1);

    const float scale = 0.01;
    const float f0 = 100.0;
    float f1 = scale * u_dt * u_size;
    float r = 1.0/(f1*5.0+1.0);

    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    frag_col = 0.9*(f0 * float(texelFetch(u_tex_density_sources, ivec2(gl_FragCoord.xy), 0)) +
                    f1 * float(texelFetch(u_tex_density_buffer, ivec2(gl_FragCoord.xy), 0).r) +
                    f1 * float(texelFetch(u_tex_density_buffer, east, 0).r) +
                    f1 * float(texelFetch(u_tex_density_buffer, west, 0).r) +
                    f1 * float(texelFetch(u_tex_density_buffer, south, 0).r) +
                    f1 * float(texelFetch(u_tex_density_buffer, north, 0).r)
                   )*r +
               0.1*float(texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy), 0));
}
