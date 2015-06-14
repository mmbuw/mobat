#include "FFT_Transformer.h"

#include "recorder.hpp"

#include "Table_Visualizer.h"

#include "microphone.hpp"
#include "locator.hpp"

#include "FFT_Transformer.h"
#include "ring_buffer.h"

#include <SFML/Graphics.hpp>

#include <iostream>





sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);

unsigned int N = 512;

unsigned int current_listened_channel = 0;

int main(int argc, char** argv) {

std::vector<Microphone> mics = {{0.0, 0.0}, {200, 0.0}, {0.0, 100.0}, {200.0, 100.0}};



Locator loc{100000, mics[0], mics[1], mics[2], mics[3]};

    mics[0].toa = 0.0003062;
    mics[1].toa = 0.0012464;
    mics[2].toa = 0.0000;
    mics[3].toa = 0.0011279;

//sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(sf::VideoMode(windowResolution.x, windowResolution.y)
						, "Table_Vis");
//window.setSize(windowResolution);

	std::vector<sf::Vector2f> default_microphone_positions_;
		default_microphone_positions_.push_back(sf::Vector2f(0.0,0.0));
		default_microphone_positions_.push_back(sf::Vector2f(0.0,8.0));
		default_microphone_positions_.push_back(sf::Vector2f(4.0,0.0));
		default_microphone_positions_.push_back(sf::Vector2f(4.0,8.0));

TTT::Table_Visualizer table_visualizer(windowResolution);
table_visualizer.Set_Token_Recognition_Timeout(10000);

std::cout << "FFT Window Size: " << N << "\n";

FFT_Transformer fft_transformer(N);

fft_transformer.initialize_execution_plan();

#define NUM_RECORDED_CHANNELS 4

Recorder recorder{NUM_RECORDED_CHANNELS, 44100, 200000};
//recorder initialization code END


std::cout << "NUM_RECORDED_CHANNELS: " << NUM_RECORDED_CHANNELS << "\n";


//int* streamed_buffer1 = (int*) malloc(recorder.buffer_bytes()/ NUM_RECORDED_CHANNELS);
//int* streamed_buffer2 = (int*) malloc(recorder.buffer_bytes() / 4 );
//int* streamed_buffer3 = (int*) malloc(recorder.buffer_bytes() / 4 );
//int* streamed_buffer4 = (int*) malloc(recorder.buffer_bytes() / 4 );

int **buffer_collector = 0;

buffer_collector 
    = (int **) malloc( NUM_RECORDED_CHANNELS * sizeof(int*));


for(unsigned int streamed_buffer_iterator = 0; streamed_buffer_iterator < NUM_RECORDED_CHANNELS; ++streamed_buffer_iterator) {
    buffer_collector[streamed_buffer_iterator] = (int*) malloc(recorder.buffer_bytes() / NUM_RECORDED_CHANNELS);
}







    unsigned frame_counter = 0;
    while (window.isOpen())
    {
        recorder.record();

        std::cout << "Buffer Length: " << recorder.buffer_bytes() << "\n";

        unsigned char* recorded_buffer = recorder.buffer();

        
        //streamed_buffer1 = (int*) recorded_buffer;
        //memcpy(streamed_buffer1, recorded_buffer, recorder.buffer_bytes()/2 );

        std::cout << "Num Recorded Channels: " << NUM_RECORDED_CHANNELS << "\n";

        for(unsigned int streamed_buffer_iterator = 0; streamed_buffer_iterator < NUM_RECORDED_CHANNELS; ++streamed_buffer_iterator) {

            std::cout << "Audiostream " << streamed_buffer_iterator << " accessor\n";
           //interleaved
           for(unsigned int buffer_offset_pos = 0; buffer_offset_pos < recorder.buffer_bytes()/(NUM_RECORDED_CHANNELS*4

            ); ++buffer_offset_pos) {
             //memcpy( &(buffer_collector[streamed_buffer_iterator][buffer_offset_pos]), &recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) ], 4 );

/*
                //interleaved 8 bit chunks
                buffer_collector[streamed_buffer_iterator][buffer_offset_pos] 
                    =     0x0 
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) ]                             << 0
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) +     NUM_RECORDED_CHANNELS ] << 8
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + 2 * NUM_RECORDED_CHANNELS ] << 16
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + 3 * NUM_RECORDED_CHANNELS ] << 24;
  
                                               */                             
                //interleaved 32 bit chunks


               // unsigned int first_pos = buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + streamed_buffer_iterator * 4 ;

                /*
                std::cout << "Accessing recorder buffer at " << first_pos << " & "
                                                             << first_pos + 1 << " & "
                                                             << first_pos + 2 << " & "
                                                             << first_pos + 3 << "\n";
                */

                buffer_collector[streamed_buffer_iterator][buffer_offset_pos] 
                    =     0x0 
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4)      + streamed_buffer_iterator * 4 ]  << 0
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + 1  + streamed_buffer_iterator * 4 ]  << 8
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + 2  + streamed_buffer_iterator * 4 ]  << 16
                        | recorded_buffer[buffer_offset_pos * (NUM_RECORDED_CHANNELS*4) + 3  + streamed_buffer_iterator * 4 ]  << 24;

                //if (buffer_offset_pos > 3)
                //break;
            }
           //std::cin.get();
            //non-interleaved

            // unsigned byte_per_channel = recorder.buffer_bytes()/NUM_RECORDED_CHANNELS;

            // memcpy( &(buffer_collector[streamed_buffer_iterator][0]), 
            //         &recorded_buffer[ byte_per_channel * streamed_buffer_iterator], 
            //         byte_per_channel );
        }

/*
        fft_transformer.set_FFT_buffers(NUM_RECORDED_CHANNELS, 
                            recorder.buffer_bytes()/NUM_RECORDED_CHANNELS,
                            (int*)&buffer_collector[0][0]);
*/
             fft_transformer.set_FFT_buffers(NUM_RECORDED_CHANNELS, 
                            recorder.buffer_bytes()/NUM_RECORDED_CHANNELS,
                            (int**)&buffer_collector[current_listened_channel]);   


//std::chrono::system_clock::time_point before_fft = std::chrono::system_clock::now();
    for(unsigned int i = 0; i < 200000; ++i) {

        unsigned offset = 10 * i;
        if(offset > 200000)
            break;


      //  fft_transf.set_FFT_input(
        //                         offset);
        fft_transformer.set_analyzation_range(0+offset, N+50 + offset);
        

        if(fft_transformer.perform_FFT() ) {
            break;
        }

     
    } 

//std::chrono::system_clock::time_point after_fft = std::chrono::system_clock::now();
//std::cout << "fftw execution time: " <<
//std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count() << "\n";



        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {

            	if(event.key.code == sf::Keyboard::Escape)
            		window.close();

            	if(event.key.code == sf::Keyboard::Up 
            		|| event.key.code == sf::Keyboard::Down
            		|| event.key.code == sf::Keyboard::Left 
            		|| event.key.code == sf::Keyboard::Right) {
	            	if (event.key.code == sf::Keyboard::Up) {

                        if(current_listened_channel < 3)
                            ++current_listened_channel;
	            	}
	            	if (event.key.code == sf::Keyboard::Down) {

                        if(current_listened_channel > 0)
                            --current_listened_channel;
	            	}
	            	if (event.key.code == sf::Keyboard::Left) {
	            	} 
	            	if (event.key.code == sf::Keyboard::Right) {
	            	}

                }
               
            }
       }



        window.clear();
        table_visualizer.Recalculate_Geometry();
        table_visualizer.Draw(window);
           
        glm::vec2 point = loc.locate();
        smartphonePosition.x = point.x / 100.0;
        smartphonePosition.y = point.y / 100.0;

        //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";
        table_visualizer.Signal_Token(18000, smartphonePosition);

        if( 0 == ++frame_counter % 40  ) {
         table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                            smartphonePosition.y+0.2));
        }

        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

return 0;
}
