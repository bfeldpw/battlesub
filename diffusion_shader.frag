uniform sampler2D u_tex_density_srcs;
uniform sampler2D u_tex_diffusion;

in vec2 v_tex;

out float fragmentColor;

void main()
{
    fragmentColor = (   float(texture(u_tex_density_srcs, v_tex + vec2( 0,  0))) +
                        float(texture(u_tex_diffusion, v_tex + vec2(-1,  0))) +
                        float(texture(u_tex_diffusion, v_tex + vec2( 1,  0))) +
                        float(texture(u_tex_diffusion, v_tex + vec2( 0, -1))) +
                        float(texture(u_tex_diffusion, v_tex + vec2( 0,  1)))
    );
}
