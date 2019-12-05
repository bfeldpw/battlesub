uniform sampler2D u_tex_densities;
uniform sampler2D u_tex_density_base;
uniform sampler2D u_tex_velocities;
uniform sampler2D u_tex_diffusion;

in vec2 v_tex;

out vec3 fragmentColor;

float diffuse_densities(ivec2 v)
{
    const float f = 100.0;
    const float r = 1.0/(f*5.0+1.0);
    const ivec3 o = ivec3(-1,0,1);
    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    return 0.9*(    float(texelFetch(u_tex_densities, ivec2(gl_FragCoord.xy), 0)) +
                f * float(texelFetch(u_tex_diffusion, ivec2(gl_FragCoord.xy) - v, 0).r) +
                f * float(texelFetch(u_tex_diffusion, east - v, 0).r) +
                f * float(texelFetch(u_tex_diffusion, west - v, 0).r) +
                f * float(texelFetch(u_tex_diffusion, south - v, 0).r) +
                f * float(texelFetch(u_tex_diffusion, north - v, 0).r)
               )*r +
           0.1*float(texelFetch(u_tex_density_base, ivec2(gl_FragCoord.xy) - v, 0));
}

vec2 diffuse_velocities(ivec2 v)
{
    const float f = 1.0;
    const float r = 1.0/(f*5.0);
    const ivec3 o = ivec3(-1,0,1);
    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    return     (/*vec2(1.0, 1.0) +*/
                f * (texelFetch(u_tex_diffusion, ivec2(gl_FragCoord.xy), 0).gb) +
                f * (texelFetch(u_tex_diffusion, east, 0).gb) +
                f * (texelFetch(u_tex_diffusion, west, 0).gb) +
                f * (texelFetch(u_tex_diffusion, south, 0).gb) +
                f * (texelFetch(u_tex_diffusion, north, 0).gb)
               )*r;
}

void main()
{
    vec2 vel   = 0.05*texelFetch(u_tex_diffusion, ivec2(gl_FragCoord.xy), 0).rg;
    fragmentColor.gb += texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).rb;
    fragmentColor.gb += diffuse_velocities(ivec2(vel));
    fragmentColor.r = diffuse_densities(ivec2(vel));
                    
// fragmentColor = float(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).y);
                     
}
