uniform sampler2D u_texture;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    float value = abs(float(texture(u_texture, v_tex).r) + float(texture(u_texture, v_tex).b));
    fragmentColor.rgb = vec3(value*0.2, value*0.3, value);
    fragmentColor.a = 1.0;
//     fragmentColor.rgba = vec4(1.0, 0.0, 0.0, 1.0);
}
