uniform vec2 mic1_pos;
uniform vec2 mic2_pos;
uniform vec2 mic3_pos;
uniform vec2 mic4_pos;

uniform vec4 toas;

const float table_width = 1.0;
const float table_height = 2.0;

float dif(vec2 first_mic_pos, vec2 second_mic_pos, float first_toa, float second_toa) {

	float dtoa = (first_toa - second_toa);

    //std::cout<<"betr "<<betrag(p , p_mic_1)<<"    "<<betrag(p , p_mic_2)<<"    "<<c*dtoa<<"\n";

    vec2 current_position = vec2(1.0,2.0)*(gl_FragCoord.xy / vec2(800, 600));
    return length(current_position - first_mic_pos) 
    	 - length(current_position - second_mic_pos) 
    	 - 3300.0 * dtoa;
}

void main()
{
	vec2 mic_pos[4];
	mic_pos[0] = mic1_pos;
	mic_pos[1] = mic2_pos;
	mic_pos[2] = mic3_pos;
	mic_pos[3] = mic4_pos;

	float temp_dif = 0.0;

    for(int mic_1 = 0; mic_1 < 3; ++mic_1) {
        for(int mic_2 = mic_1+1; mic_2 < 4; ++mic_2) {

            if( (toas[mic_1] < 10000.0) || (toas[mic_1] < 10000.0) )
                  temp_dif += abs(dif( mic_pos[mic_1] , mic_pos[mic_2], toas[mic_1], toas[mic_2]));
        }
    }


    if(temp_dif > 50000.0)
		gl_FragColor = vec4(0.0, temp_dif/5.0, 0.0, 1.0);
    else {
		gl_FragColor = vec4(temp_dif, 0.0, 0.0, 1.0);
		//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    }

    //gl_FragColor = vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 0.0, 1.0);
}
