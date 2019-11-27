uniform vec3 u_col = vec3(1.0, 1.0, 1.0);
uniform sampler2D u_tex_density_srcs;
uniform sampler2D u_tex_diffusion;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    fragmentColor.b = (float(texture(u_tex_density_srcs, v_tex)));
    fragmentColor.rga = vec3(0.0, 0.0, 1.0);
}
