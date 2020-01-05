uniform sampler2D u_texture;

uniform bool  u_show_only_magnitude;
uniform float u_scale;

in vec2 v_tex;

out vec4 frag_col;

void main()
{
    // Scale velocity to interval [0, 1/scale], e.g. scale = 0.05 => [0, 20] m/s
    float x = texture(u_texture, v_tex).r;
    float y = texture(u_texture, v_tex).g;
    
    // Show only magnitude of velocity
    if (u_show_only_magnitude)
    {
        vec2 vec = vec2(x,y) * u_scale;
        frag_col = vec4(length(vec), length(vec), length(vec), 1.0);
    }
    // Or show colour-coded direction and magnitude
    else
    {
        // Very simply colour coding for direction (not orientation)
        float scale_sqr = u_scale * u_scale;
        float b = x*y;
        frag_col.rgba = vec4(u_scale*abs(x),
                             u_scale*abs(y),
                             scale_sqr*b,
                             1.0);
    }
}
