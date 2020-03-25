uniform sampler2D u_tex_density_buffer;
uniform sampler2D u_tex_velocities;

uniform float u_advection_factor;
uniform float u_dt;
uniform float u_grid_res;

in vec2 v_tex;

out float frag_col;

void main()
{
    vec2 vel = u_advection_factor*texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 pos = gl_FragCoord.xy - u_grid_res * u_dt * vel;

    // Neighbour indices
    vec4 n;
    n.xy = floor(pos-0.5)+0.5;
    n.zw = n.xy+1.0;
        
    // Neighbour values
    float v_tl = texelFetch(u_tex_density_buffer, ivec2(n.xy), 0).x; // top left
    float v_tr = texelFetch(u_tex_density_buffer, ivec2(n.zy), 0).x; // top right
    float v_bl = texelFetch(u_tex_density_buffer, ivec2(n.xw), 0).x; // bottom left
    float v_br = texelFetch(u_tex_density_buffer, ivec2(n.zw), 0).x; // bottom right

    vec2 f = pos - n.xy;

    frag_col = mix(mix(v_tl, v_tr, f.x), mix(v_bl, v_br, f.x), f.y);
}
