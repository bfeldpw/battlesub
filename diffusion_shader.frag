uniform sampler2D u_tex_density_srcs;
uniform sampler2D u_tex_diffusion;

in vec2 v_tex;

out float fragmentColor;

void main()
{
    fragmentColor = float(texture(u_tex_density_srcs, v_tex));
}
