uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    fragmentColor.b = float(texture(u_texture, v_tex));
}
