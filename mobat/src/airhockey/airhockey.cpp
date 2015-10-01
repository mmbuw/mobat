#include "airhockey.hpp"
#include "packet.hpp"
#include "address.hpp"



Airhockey::Airhockey()
    :   receiver_(1337)
    ,   game_()
{}

Airhockey::Airhockey(std::uint16_t port)
    :   receiver_(port)
    ,   game_()
{
    receiver_.setReceiveCallback(&Airhockey::forwardToken, *this);
    receiver_.startListening();
    // std::cout<<"Listening\n";
}


void Airhockey::forwardToken(token_packet const& in){
    game_.signalToken(in.id, glm::vec2{in.x, in.y});

}


int main(int argc, char** argv) {
// std::cout<<"TEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSTTTTTTTTTTTTTTTTTTTTTTT";
    // instance.receiver_.set_receive_callback(forwardToken);
    if (argc > 2) {
      std::cout << "sender" << std::endl;
      Socket socket{6666};
      Address_ip4 target_address{127, 0, 0, 1, 30000};

      token_packet test{1, 1337.0f, 1138.0f, 0};
      packet::send<token_packet>(socket, target_address, test);
      packet::send<token_packet>(socket, target_address, test);
      packet::send<token_packet>(socket, target_address, test);
    }
    else {
      std::cout << "receiver" << std::endl;








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
        
        std::uint16_t port = 30000;

        Airhockey instance(port);



        TTT::Game game(microphone_positions_);
        instance.game_.setTokenRecognitionTimeout(configurator().getUint("recognition_timeout"));

        instance.game_.setTokenFillColor(configurator().getUint("player1"), sf::Color(0,0,255) );
        instance.game_.setTokenFillColor(configurator().getUint("player2"), sf::Color(255, 0, 0) );
            
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


        // time_t starttime = time(0);
        // time_t endtime = starttime +60;
        
        

        while (window.isOpen()) {
            if(window.pollEvent(event)) {
                // toggle game
                if(event.key.code == sf::Keyboard::Space){
                    if (event.type == sf::Event::KeyPressed){
                        instance.game_.toggleGame();
                    }
                }
            }



            if(!instance.game_.gameOver().first){
                if(show_tablevis) {
                    window.clear();

                    instance.game_.handleKeyboardInput();
                    

                    instance.game_.recalculateGeometry();
                    
                    

                    // instance.game_.draw(window);
                }




                instance.game_.draw(window);

                // instance.game_.writeTokens();
                



    //TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING 
                    // token_packet test_tp{19000, 0.5, 0.5, 123};
                    // instance.forwardToken(test_tp);
    //TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING 



                

                if(show_tablevis) {
                
                    instance.game_.updateTokens();


                }


                
            
            }else {
                
                    if(draw_endscreen_){

                        winner = instance.game_.gameOver().second;

                       if(winner == "Red"){
                            window.draw(rect_red);
                        }
                        else{
                            window.draw(rect_blue);
                        }

                        draw_endscreen_ = false;
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !instance.game_.gameActive()) {
                        draw_endscreen_ = true;
                        instance.game_.restart();
                    }
                
            }
            
            if(show_tablevis) {
                window.display();  
            }
        }

    }
   
    return 0;


}