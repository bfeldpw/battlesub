uniform sampler2D u_tex_velocity_buffer;

uniform float u_advection_factor;
uniform float u_dt;
uniform float u_grid_res;

in vec2 v_tex;

out vec2 frag_col;

void main()
{
    vec2 pos = gl_FragCoord.xy - u_advection_factor * u_grid_res * u_dt *
               texelFetch(u_tex_velocity_buffer, ivec2(gl_FragCoord.xy), 0).xy;

    // Neighbour indices
    vec4 n;
    n.xy = floor(pos-0.5)+0.5;
    n.zw = n.xy+1.0;
        
    // Neighbour values
    vec2 v_tl = texelFetch(u_tex_velocity_buffer, ivec2(n.xy), 0).xy; // top left
    vec2 v_tr = texelFetch(u_tex_velocity_buffer, ivec2(n.zy), 0).xy; // top right
    vec2 v_bl = texelFetch(u_tex_velocity_buffer, ivec2(n.xw), 0).xy; // bottom left
    vec2 v_br = texelFetch(u_tex_velocity_buffer, ivec2(n.zw), 0).xy; // bottom right

    vec2 f = pos - n.xy;

    frag_col = mix(mix(v_tl, v_tr, f.x), mix(v_bl, v_br, f.x), f.y);
}
