uniform sampler2D u_tex_ground;
uniform sampler2D u_tex_velocities;

uniform float u_distortion;
uniform float u_dt;
uniform float u_grid_res;
uniform float u_time;

in vec2 v_tex;

out float frag_col;

void main()
{
    vec2  vel   = texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).xy;
    float vel_v = length(vel);
    vec2  pos   = gl_FragCoord.xy - u_grid_res * u_dt * 0.2 * u_distortion *
                  (vel_v  * sin(u_time*0.1*vel_v));

    // Neighbour indices
    vec4 n;
    n.xy = floor(pos-0.5)+0.5;
    n.zw = n.xy+1.0;
        
    // Neighbour values
    float v_tl = texelFetch(u_tex_ground, ivec2(n.xy), 0).x; // top left
    float v_tr = texelFetch(u_tex_ground, ivec2(n.zy), 0).x; // top right
    float v_bl = texelFetch(u_tex_ground, ivec2(n.xw), 0).x; // bottom left
    float v_br = texelFetch(u_tex_ground, ivec2(n.zw), 0).x; // bottom right

    vec2 f = pos - n.xy;

    frag_col = mix(mix(v_tl, v_tr, f.x), mix(v_bl, v_br, f.x), f.y);
}
