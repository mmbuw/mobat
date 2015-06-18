uniform vec2 mic1_pos;
uniform vec2 mic2_pos;
uniform vec2 mic3_pos;
uniform vec2 mic4_pos;

uniform vec4 toas;


void main()
{
    gl_FragColor = vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 0.0, 1.0);
}