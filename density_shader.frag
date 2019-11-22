uniform vec3 u_col = vec3(1.0, 1.0, 1.0);
uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    fragmentColor.rgb = u_col*texture(u_texture, v_tex).rgb;
    fragmentColor.a = 1.0;
}
