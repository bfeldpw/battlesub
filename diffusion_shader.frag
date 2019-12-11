uniform sampler2D u_tex_densities;
uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_diffusion;
uniform sampler2D u_tex_velocities;

uniform float u_dt;
uniform int   u_size;

in vec2 v_tex;

out float frag_col;

void main()
{
    vec2 vel = vec2(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0));

    const ivec3 o = ivec3(-1,0,1);

    const float scale = 0.05;
    float f1 = scale * u_dt * u_size;
    float f0 = 0.05*f1;
    float r = 1.0/(f1*5.0+1.0);
    ivec2 v  = ivec2(vel*60 * u_dt);

    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy-v;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz-v;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx-v;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy-v;

    frag_col = 0.9*(f0 * float(texelFetch(u_tex_densities, ivec2(gl_FragCoord.xy), 0)) +
                    f1 * float(texelFetch(u_tex_diffusion, ivec2(gl_FragCoord.xy) - v, 0).r) +
                    f1 * float(texelFetch(u_tex_diffusion, east, 0).r) +
                    f1 * float(texelFetch(u_tex_diffusion, west, 0).r) +
                    f1 * float(texelFetch(u_tex_diffusion, south, 0).r) +
                    f1 * float(texelFetch(u_tex_diffusion, north, 0).r)
                   )*r +
               0.1*float(texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy)-v, 0));
}
