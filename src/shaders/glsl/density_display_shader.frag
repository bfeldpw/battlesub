uniform sampler2D u_texture;

uniform float u_scale;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    float value = float(texture(u_texture, v_tex));
    fragmentColor.rgb = u_scale*vec3(value*0.01, value*0.3, value);
    // fragmentColor.rgb = 0.01*vec3(value*0.01, value*0.3, value);
    fragmentColor.a = 1.0;
}
