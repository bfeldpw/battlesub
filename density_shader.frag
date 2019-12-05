uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    float value = float(texture(u_texture, v_tex).r);
    fragmentColor.rgb = vec3(value*0.2, value*0.3, value);
    fragmentColor.a = 1.0;
//     fragmentColor.rga = vec3(0.0, 0.0, 1.0);
}
