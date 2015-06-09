#include "FFT_Transformer.h"

#include "recorder.hpp"

#include "Table_Visualizer.h"


#include <SFML/Graphics.hpp>

#include <iostream>


sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);
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

Recorder recorder{1, 44100, 400000};
//recorder initialization code END


  unsigned signal_counter = 0;
    while (window.isOpen())
    {
        recorder.record();

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
