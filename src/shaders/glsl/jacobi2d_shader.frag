// Solves Ax = b
uniform sampler2D u_tex_x;
uniform sampler2D u_tex_b;

uniform float u_alpha;
uniform float u_r_beta;
uniform float u_diffusion_rate;

in vec2 v_tex;

out vec2 frag_value;

void main()
{
    const ivec3 o = ivec3(-1,0,1);

    ivec2 i_w = ivec2(gl_FragCoord.xy)+o.xy;
    ivec2 i_n = ivec2(gl_FragCoord.xy)+o.yz;
    ivec2 i_s = ivec2(gl_FragCoord.xy)+o.yx;
    ivec2 i_e = ivec2(gl_FragCoord.xy)+o.zy;

    frag_value = (u_alpha * texelFetch(u_tex_b, ivec2(gl_FragCoord.xy), 0).xy +
                  u_diffusion_rate * texelFetch(u_tex_x, i_e, 0).xy +
                  u_diffusion_rate * texelFetch(u_tex_x, i_w, 0).xy +
                  u_diffusion_rate * texelFetch(u_tex_x, i_s, 0).xy +
                  u_diffusion_rate * texelFetch(u_tex_x, i_n, 0).xy
                 ) * u_r_beta;
}
