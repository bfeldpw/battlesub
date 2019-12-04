uniform sampler2D u_tex_densities;
uniform sampler2D u_tex_velocities;
uniform sampler2D u_tex_diffusion;

in vec2 v_tex;

out float fragmentColor;

void main()
{
    const float f = 100.0;
    const float r = 1.0/(f*5.0+1.0);
    const ivec3 o = ivec3(-1,0,1);
    ivec2 vel   = ivec2(0.05*texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0));
    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;
    
    fragmentColor = (    float(texelFetch(u_tex_densities, ivec2(gl_FragCoord.xy), 0)) +
                     f * float(texelFetch(u_tex_diffusion, ivec2(gl_FragCoord.xy) - vel, 0)) +
                     f * float(texelFetch(u_tex_diffusion, east - vel, 0)) +
                     f * float(texelFetch(u_tex_diffusion, west - vel, 0)) +
                     f * float(texelFetch(u_tex_diffusion, south - vel, 0)) +
                     f * float(texelFetch(u_tex_diffusion, north - vel, 0))
                    )*r;
// fragmentColor = float(texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).y);
                     
}
