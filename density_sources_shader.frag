in float v_amount;

out float fragmentColor;

void main()
{
    fragmentColor = v_amount*300.0;
//     fragmentColor.rga = vec3(1.0, 0.0, 0.0);
}
