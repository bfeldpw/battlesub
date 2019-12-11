uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    float x = texture(u_texture, v_tex).r;
    float y = texture(u_texture, v_tex).g;
    float b = x*y;
    
    fragmentColor.r = abs(x);
    fragmentColor.g = abs(y);
    fragmentColor.b = b;
    fragmentColor.a = 1.0;
}
