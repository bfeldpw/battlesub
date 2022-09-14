uniform sampler2D u_texture;

uniform float u_scale;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    // float value = float(texture(u_texture, v_tex));
    // fragmentColor.rgb = u_scale*vec3(value*0.01, value*0.3, value);
    vec4 value = texture(u_texture, v_tex) * u_scale;

    fragmentColor.r = value.r;
    fragmentColor.g = value.g;
    fragmentColor.b = value.b;
    fragmentColor.a = 1.0;
}
