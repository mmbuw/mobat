#include "locator.hpp"
#include "Table_Visualizer.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>

#define NUM_RECORDED_CHANNELS 4

sf::Vector2f smartphonePosition(1.0f,0.5f);
sf::Vector2u windowResolution(800, 800);


int main(int argc, char** argv) {

//sf::VideoMode fullScreenMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(sf::VideoMode(windowResolution.x, windowResolution.y)
                        , "Table_Vis");
//window.setSize(windowResolution);

    std::vector<sf::Vector2f> default_microphone_positions_ = {{0.02, 1-0.02}, {0.02, 1-0.47}, {0.655, 1-0.47}, {0.665, 1-0.015}};
       /* default_microphone_positions_.push_back(sf::Vector2f(0.0,4.0));
        default_microphone_positions_.push_back(sf::Vector2f(0.0,8.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,0.0));
        default_microphone_positions_.push_back(sf::Vector2f(4.0,8.0));*/



    TTT::Table_Visualizer table_visualizer(windowResolution, sf::Vector2f(2.0, 1.0), default_microphone_positions_);
    table_visualizer.Set_Token_Recognition_Timeout(10000);

    Locator locator{4};

    auto recording_thread = std::thread(&Locator::record_position, &locator);

    //unsigned frame_counter = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {

            	if(event.key.code == sf::Keyboard::Escape)
            		window.close();

            	if(event.key.code == sf::Keyboard::Up 
            		|| event.key.code == sf::Keyboard::Down
            		|| event.key.code == sf::Keyboard::Left 
            		|| event.key.code == sf::Keyboard::Right) {
	            	// if (event.key.code == sf::Keyboard::Up) {

              //           if(current_listened_channel < 3)
              //               ++current_listened_channel;
	            	// }
	            	// if (event.key.code == sf::Keyboard::Down) {

              //           if(current_listened_channel > 0)
              //               --current_listened_channel;
	            	// }
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
           
        glm::vec2 point = locator.load_position();
        smartphonePosition.x = point.x / 100.0;
        smartphonePosition.y = 1.0 - point.y / 100.0;

        //std::cout << "SP: " << smartphonePosition.x << "; " << smartphonePosition.y << "\n";
        table_visualizer.Signal_Token(18000, smartphonePosition);

/*
        if( 0 == ++frame_counter % 40  ) {
         table_visualizer.Signal_Token(16000, sf::Vector2f(smartphonePosition.x/2.0,
                                                            smartphonePosition.y+0.2));
        }
*/
        table_visualizer.Finalize_Visualization_Frame();
        window.display();
    }

    locator.shut_down();
    recording_thread.join();

    return 0;
}
