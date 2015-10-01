
#include "game.hpp"
#include "drawable_object.hpp"
#include "score.hpp"
#include "configurator.hpp"

#include "../udp_receiver/udp_receiver.hpp"


#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <X11/Xlib.h>






int main(int argc, char** argv) {
//######################################RECEVER##########################################
    TTT::UdpReceiver receiver;
    glm::vec2 rcv_test_pos{0.23, 0.45};
    std::pair<unsigned, glm::vec2> rcv_test_entry{123, rcv_test_pos};
    receiver.receive(rcv_test_entry);

    glm::vec2 rcv_test_pos2{0.5, 1.0};
    std::pair<unsigned, glm::vec2> rcv_test_entry2{13, rcv_test_pos2};
    receiver.receive(rcv_test_entry2);

    receiver.printAll();
    receiver.print(123);
    auto asdf = receiver.positionOf(123);
    std::cout << asdf.x << ", " << asdf.y << "\n";
    
    auto rcv_test_dir = receiver.directionFromTo(123, 13);
    std::cout<< rcv_test_dir.x <<", " << rcv_test_dir.y <<"\n";

//########################################################################################
    std::string file_name{"default.conf"};
    if(argc > 1) {
        file_name = argv[1];
    }

    configurator().read(file_name);

    glm::vec2 windowResolution{configurator().getVec("resolution")};


    std::vector<unsigned> frequencies_to_record{configurator().getList("known_frequencies")};


// calculation


// visualisation

    
    
    bool show_tablevis = configurator().getUint("show_tablevis") > 0;

    //std::vector<unsigned> const known_frequencies = configurator().getList("known_frequencies");

    
    

    sf::ContextSettings settings;
    //settings.antialiasingLevel = 8;

    sf::Uint32 style = sf::Style::Default;
    if(configurator().getUint("fullscreen") > 0) {
        style = sf::Style::Fullscreen;
    }

    sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                           , "Table_Vis", style, settings);

    // Limit the framerate to 60 frames per second (this step is optional)
    window.setFramerateLimit(60);
    

    std::vector<glm::vec2> microphone_positions_ = {configurator().getVec("microphone1_pos"),
                                                    configurator().getVec("microphone2_pos"),
                                                    configurator().getVec("microphone3_pos"),
                                                    configurator().getVec("microphone4_pos")};

    // initialize measures for drawing & simulation 
    TTT::DrawableObject::setBasis(configurator().getVec("right"), configurator().getVec("up"));
    TTT::DrawableObject::setResolution(windowResolution);
    TTT::DrawableObject::setPhysTableSize(configurator().getVec("table_size"));
    TTT::DrawableObject::setProjection(configurator().getVec("projection_offset"), configurator().getVec("projection_size"));

    TTT::Game game(microphone_positions_);
    game.setTokenRecognitionTimeout(configurator().getUint("recognition_timeout"));

    game.setTokenFillColor(configurator().getUint("player1"), sf::Color(0,0,255) );
    game.setTokenFillColor(configurator().getUint("player2"), sf::Color(255, 0, 0) );
        
// game
    std::string winner;

    sf::Event event;
        
    sf::Texture texture_red;
    texture_red.loadFromFile("../pictures/red_wins.png"); 
    sf::Texture texture_blue;
    texture_blue.loadFromFile("../pictures/blue_wins.png"); 

    sf::RectangleShape rect_red;
    rect_red.setSize(sf::Vector2f(600, 400));
    rect_red.setPosition(sf::Vector2f(windowResolution.x/2 - 300, windowResolution.y/2 - 200));
    rect_red.setTexture(&texture_red, false);
    sf::RectangleShape rect_blue{rect_red};
    rect_blue.setTexture(&texture_blue, false);

    bool draw_endscreen_ = true;



    
    

    while (window.isOpen()) {
        if(window.pollEvent(event)) {
            // toggle game
            if(event.key.code == sf::Keyboard::Space){
                if (event.type == sf::Event::KeyPressed){
                    game.toggleGame();
                }
            }
        }



        if(!game.gameOver().first){
            if(show_tablevis) {
                window.clear();

                game.handleKeyboardInput();

                game.recalculateGeometry();
                
                

                game.draw(window);
            }
            // #############################RECEIVING##################################################################
            receiver.draw(window);
//          ################################END RECEIVING##############################################################
         
            
            

            if(show_tablevis) {
            
//##############################################################################################
                receiver.receive(rcv_test_entry2);
                receiver.updateTokens();
//##############################################################################################
            }


            
        
        }else {
            
                if(draw_endscreen_){

                    winner = game.gameOver().second;

                   if(winner == "Red"){
                        window.draw(rect_red);
                    }
                    else{
                        window.draw(rect_blue);
                    }

                    draw_endscreen_ = false;
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !game.gameActive()) {
                    draw_endscreen_ = true;
                    game.restart();
                }
            
        }
        
        if(show_tablevis) {
            window.display();  
        }
    }

   
    

    return 0;
}




