uniform sampler2D u_texture;

uniform float u_gamma;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    float value = float(texture(u_texture, v_tex).r);
    fragmentColor.rgb = vec3(value*0.01, value*0.01, value);
    fragmentColor.a = 1.0;
    
    fragmentColor.rgb = pow(fragmentColor.rgb, vec3(1.0/u_gamma));
}
