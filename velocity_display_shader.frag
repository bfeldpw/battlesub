uniform sampler2D u_texture;

uniform float u_scale;

in vec2 v_tex;

out vec4 fragmentColor;

void main()
{
    // Scale velocity to interval [0, 1/scale], e.g. scale = 0.05 => [0, 20] m/s
    float scale_sqr = u_scale * u_scale;

    // Very simply color coding for direction (not orientation)
    float x = texture(u_texture, v_tex).r;
    float y = texture(u_texture, v_tex).g;
    float b = x*y;
    
//     fragmentColor.rgba = vec4(u_scale*abs(x),
//                               u_scale*abs(y),
//                               scale_sqr*b,
//                              1.0);
    vec2 vec = vec2(x,y) * u_scale;
    fragmentColor = vec4(length(vec), length(vec), length(vec), 1.0);
}
