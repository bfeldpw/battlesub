uniform sampler2D u_tex_velocity_sources;
uniform sampler2D u_tex_velocity_buffer;

uniform float u_dt;
uniform int   u_grid_res;

in vec2 v_tex;

out vec2 frag_col;

void main()
{
    // Weight of source
    float weight = texelFetch(u_tex_velocity_sources, ivec2(gl_FragCoord.xy), 0).z;

    // Normalisation
    float f_n = u_dt * u_grid_res; // Normalisation
    float f_w = f_n * weight;      // Weight
    float f_z = f_n * 0.0;         // Dissolve to zero rate

    float r = 1.0/(f_w+5.0*f_n+f_z);

    const ivec3 o = ivec3(-1,0,1);
    ivec2 west  = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 north = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 south = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 east  = ivec2(gl_FragCoord.xy)+o.zy;

    frag_col = (f_w * (texelFetch(u_tex_velocity_sources, ivec2(gl_FragCoord.xy), 0)).xy +
                f_n * (texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy), 0)).xy +
                f_n * (texelFetch(u_tex_velocity_buffer, east, 0)).xy +
                f_n * (texelFetch(u_tex_velocity_buffer, west, 0)).xy +
                f_n * (texelFetch(u_tex_velocity_buffer, south, 0)).xy +
                f_n * (texelFetch(u_tex_velocity_buffer, north, 0)).xy
               )*r;
}
