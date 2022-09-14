uniform sampler2D u_tex_buffer;
uniform sampler2D u_tex_velocities;

uniform float u_advection_factor;
uniform float u_dt;
uniform float u_grid_res;

in vec2 v_tex;

out vec3 frag_value;

void main()
{
    vec2 vel = u_advection_factor*texelFetch(u_tex_velocities, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 pos = gl_FragCoord.xy - u_grid_res * u_dt * vel;

    // Neighbour indices
    vec4 n;
    n.xy = floor(pos-0.5)+0.5;
    n.zw = n.xy+1.0;
        
    // Neighbour values
    vec3 v_tl = texelFetch(u_tex_buffer, ivec2(n.xy), 0).rgb; // top left
    vec3 v_tr = texelFetch(u_tex_buffer, ivec2(n.zy), 0).rgb; // top right
    vec3 v_bl = texelFetch(u_tex_buffer, ivec2(n.xw), 0).rgb; // bottom left
    vec3 v_br = texelFetch(u_tex_buffer, ivec2(n.zw), 0).rgb; // bottom right

    vec2 f = pos - n.xy;

    frag_value = mix(mix(v_tl, v_tr, f.x), mix(v_bl, v_br, f.x), f.y);
}
