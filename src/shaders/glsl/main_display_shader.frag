uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 frag_color;

void main()
{
    frag_color = texture(u_texture, v_tex);
}
