uniform sampler2D u_tex_velocities;
uniform sampler2D u_tex_velocity_buffer;

uniform float u_dt;
uniform int   u_size;

in vec2 v_tex;

out vec2 frag_col;

void main()
{
    const ivec3 o = ivec3(-1,0,1);

    const float scale = 1.05;
    float f1 = scale * u_dt * u_size;
    float f0 = 0.5*f1;
    float r = 1.0/(f1*5.0+1.0);
    
    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    frag_col = vec2(f0 * (texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy), 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, east, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, west, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, south, 0)) +
                    f1 * (texelFetch(u_tex_velocity_buffer, north, 0))
                   )*r;
}
