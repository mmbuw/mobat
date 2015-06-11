#include "FFT_Transformer.h"

#include "recorder.hpp"

#include "Table_Visualizer.h"

#include "FFT_Transformer.h"
#include "ring_buffer.h"

#include <SFML/Graphics.hpp>

#include <iostream>


sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);

unsigned int N = 2048;

int main(int argc, char** argv) {

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

//table_visualizer.Set_Table_Fill_Color(sf::Color(82,159,153));
//table_visualizer.Recalculate_Geometry((sf::Vector2f)window.getSize());



//recorder initialization code START
//int err;

//std::vector<std::string> devices{Recorder::get_pcms()};
 
//Config playback_config{1, 44100, 23219};
//std::vector<std::string> playback_devices{Recorder::get_supporting_devices(devices, playback_config, SND_PCM_STREAM_PLAYBACK)};

//device playback_device{playback_devices[0], SND_PCM_STREAM_PLAYBACK};
//if(!playback_device) return 1;

//playback_config.install(playback_device);

std::cout << "FFT Window Size: " << N << "\n";

FFT_Transformer fft_transformer(N);

fft_transformer.initialize_execution_plan();



Recorder recorder{4, 44100, 4000000};
//recorder initialization code END


int* int_buffer = (int*) malloc(recorder.buffer_bytes() );

int **buffer_collector = 0;

buffer_collector 
    = (int **) malloc(4*sizeof(int*));

buffer_collector[0] = int_buffer;

fft_transformer.set_FFT_buffers(1, 
                            recorder.buffer_bytes()/4,
                            (int**)buffer_collector);


fft_transformer.set_FFT_input(0);



  unsigned signal_counter = 0;
    while (window.isOpen())
    {
        recorder.record();

        std::cout << "Buffer Length: " << recorder.buffer_bytes() << "\n";

        unsigned char* recorded_buffer = recorder.buffer();

        
        int_buffer = (int*) recorded_buffer;

        //int first_int = *(reinterpret_cast<int *>(&recorded_buffer[4]));
        std::cout << (char)recorded_buffer[2] <<"\n";
        std::cout << "First int: " << int_buffer[0] << "\n";




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

        //fft_transf.print_FFT_result(i);

     
    } 

std::chrono::system_clock::time_point after_fft = std::chrono::system_clock::now();
std::cout << "fftw execution time: " <<
std::chrono::duration_cast<std::chrono::microseconds>(after_fft - before_fft).count() << "\n";
    std::cout << "After setting input\n";



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

		if(++signal_counter < 3 ) {
			//std::cout << "signaling\n";

        	table_visualizer.Signal_Token(18000, smartphonePosition);
        } else {

            if(signal_counter < 5) {

            } else {
                smartphonePosition.x = 2.0* std::rand() / (float)(RAND_MAX);
                smartphonePosition.y = 1.0* std::rand() / (float)(RAND_MAX);
                signal_counter = 0;
            }

        	//table_visualizer.Signal_Token(18000, sf::Vector2f(1.0f, 0.5f));
        	//std::rand()/RAND_MAX
        	//signal_counter = 0;
        }

        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

return 0;
}
