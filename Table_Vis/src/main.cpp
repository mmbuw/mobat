#include "../include/Table_Visualizer.h"

#include <SFML/Graphics.hpp>

#include <iostream>


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

    while (window.isOpen())
    {

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
        window.display();
    }

return 0;
}
