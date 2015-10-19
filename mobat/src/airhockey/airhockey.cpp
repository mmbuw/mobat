#include "airhockey.hpp"
#include "packet.hpp"
#include "address.hpp"

using namespace MoB;

Airhockey::Airhockey(std::uint16_t port)
    :   receiver_(port)
    ,   game_() {
    receiver_.setReceiveCallback(&Airhockey::forwardToken, *this);
    receiver_.startListening();
}


void Airhockey::forwardToken(token_packet const& in){
    game_.signalToken(in.id, glm::vec2{in.x, in.y});
}

int main(int argc, char** argv) {


  std::cout << "Started Airhockey_Demo" << std::endl;
    
  std::string file_name{"default_airhockey.conf"};
    
  if(argc > 1) {
    file_name = argv[1];
  }

  configurator().read(file_name);

  glm::vec2 windowResolution{configurator().getVec("resolution")};

  sf::ContextSettings settings;


  sf::Uint32 style = sf::Style::Default;
  if(configurator().getUint("fullscreen") > 0) {
    style = sf::Style::Fullscreen;
  }


  sf::RenderWindow window(sf::VideoMode(unsigned(windowResolution.x), unsigned(windowResolution.y))
                          , "Table_Vis", style, settings);

  // Limit the framerate to 60 frames per second (this step is optional)
  window.setFramerateLimit(60);
        

  // initialize measures for drawing & simulation 
  DrawableObject::setBasis(configurator().getVec("right"), configurator().getVec("up"));
  DrawableObject::setResolution(windowResolution);
  DrawableObject::setPhysTableSize(configurator().getVec("table_size"));
  DrawableObject::setProjection(configurator().getVec("projection_offset"), configurator().getVec("projection_size"));
        
  std::uint16_t port = configurator().getUint("receiver_port");

  Airhockey instance(port);

  MoB::Game game(std::vector<glm::vec2>());
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
      window.clear();

      instance.game_.handleKeyboardInput();      
      instance.game_.recalculateGeometry();
      instance.game_.draw(window);  
      instance.game_.updateTokens();
    } else {
      if(draw_endscreen_){
        winner = instance.game_.gameOver().second;
        if(winner == "Red"){
          window.draw(rect_red);
        } else {
          window.draw(rect_blue);
        }
          draw_endscreen_ = false;
        }
                   
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || !instance.game_.gameActive()) {
        draw_endscreen_ = true;
        instance.game_.restart();
      }
                
    }
            
    window.display();  
        
    }

  return 0;
}