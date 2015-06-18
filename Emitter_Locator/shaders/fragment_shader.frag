uniform vec2 mic1_pos;
uniform vec2 mic2_pos;
uniform vec2 mic3_pos;
uniform vec2 mic4_pos;

uniform vec4 toas;


void main()
{
    if(mic1_pos.x > 0.011 && mic1_pos.x < 0.1)
	gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);

    if(mic2_pos.x > 0.011 && mic3_pos.x < 0.1)
	gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);

    //gl_FragColor = vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 0.0, 1.0);
}
