#ifndef SCORE_H
#define SCORE_H

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>


namespace TTT {



class Score{
  private:

    sf::Vector2f position_;
    std::array<sf::RectangleShape, 6> points_;
  	

  //virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

  public:
    Score(){};
    ~Score(){};
    Score(double x_max, double y_max);
    void Draw(sf::RenderWindow& canvas) const;
    void update(int r_goals, int l_goals); //changes colors according to given scores
    void reset();
    
    
    
   
    
    
    
  

    

    
    

    



};

};//Namespace TTT

#endif