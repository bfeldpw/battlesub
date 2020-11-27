uniform sampler2D u_texture;

in vec2 v_tex;

out vec2 frag_col;

void main()
{
    frag_col.xy = texture(u_texture, v_tex).xy;
}
