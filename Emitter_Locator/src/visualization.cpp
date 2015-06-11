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

int main(int argc, char** argv) {

std::vector<Microphone> mics = {{0.0, 0.0}, {200, 0.0}, {0.0, 100.0}, {200.0, 100.0}};



Locator loc{100000, mics[0], mics[1], mics[2], mics[3], 0.0, 200.4, 0.0, 150.9};

    mics[0].set_toa(0.0003062);
    mics[1].set_toa(0.0012464);
    mics[2].set_toa(0.0000);
    mics[3].set_toa(0.0011279);

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



Recorder recorder{1, 44100, 400000};
//recorder initialization code END


int* streamed_buffer1 = (int*) malloc(recorder.buffer_bytes() );
//int* streamed_buffer2 = (int*) malloc(recorder.buffer_bytes() / 4 );
//int* streamed_buffer3 = (int*) malloc(recorder.buffer_bytes() / 4 );
//int* streamed_buffer4 = (int*) malloc(recorder.buffer_bytes() / 4 );

int **buffer_collector = 0;

buffer_collector 
    = (int **) malloc(4*sizeof(int*));







  //unsigned signal_counter = 0;
    while (window.isOpen())
    {
        recorder.record();

        std::cout << "Buffer Length: " << recorder.buffer_bytes() << "\n";

        unsigned char* recorded_buffer = recorder.buffer();

        
        //streamed_buffer1 = (int*) recorded_buffer;
        memcpy(streamed_buffer1, recorded_buffer, recorder.buffer_bytes() );


       /* for(unsigned int buffer_offset_pos = 0; buffer_offset_pos < recorder.buffer_bytes()/4; ++buffer_offset_pos) {
            memcpy(&streamed_buffer1[buffer_offset_pos], &recorded_buffer[buffer_offset_pos], 4 );
        }
*/
        //int first_int = *(reinterpret_cast<int *>(&recorded_buffer[4]));
        std::cout << (char)recorded_buffer[2] <<"\n";
        std::cout << "First int: " << streamed_buffer1[0] << "\n";

        buffer_collector[0] = streamed_buffer1;


        fft_transformer.set_FFT_buffers(1, 
                            recorder.buffer_bytes()/4,
                            (int**)buffer_collector);


        //fft_transformer.set_FFT_input(0);



std::chrono::system_clock::time_point before_fft = std::chrono::system_clock::now();
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

std::chrono::system_clock::time_point after_fft = std::chrono::system_clock::now();
std::cout << "fftw execution time: " <<
std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count() << "\n";



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
	            	}
	            	if (event.key.code == sf::Keyboard::Down) {
	            	}
	            	if (event.key.code == sf::Keyboard::Left) {
	            	} 
	            	if (event.key.code == sf::Keyboard::Right) {
	            	}

                }
               
            }
       }
       std::cout << window.getSize().x << ", " << window.getSize().y << "\n";
        window.clear();
table_visualizer.Recalculate_Geometry();
        table_visualizer.Draw(window);

           
            glm::vec2 point = loc.locate();
            smartphonePosition.x = point.x / 100.0;
            smartphonePosition.y = point.y / 100.0;

            std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";
            table_visualizer.Signal_Token(18000, smartphonePosition);
        	//table_visualizer.Signal_Token(18000, sf::Vector2f(1.0f, 0.5f));
        	//std::rand()/RAND_MAX
        	//signal_counter = 0;
        

        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

return 0;
}
