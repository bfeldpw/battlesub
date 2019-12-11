uniform sampler2D u_tex_velocities;
uniform sampler2D u_tex_velocity_buffer;

uniform float u_dt;
uniform int   u_size;

in vec2 v_tex;

out vec2 frag_col;

void main()
{
    vec2 vel = texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy), 0).rg;

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

    frag_col = vec2(f0 * (texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy - v), 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, east, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, west, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, south, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, north, 0))
                   )*r;
}
