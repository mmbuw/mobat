#ifndef POINTS_H
#define POINTS_H

#include "Drawable_Object.h"
#include <glm/glm.hpp>
#include <iostream>

namespace TTT {




class Points : Drawable_Object {
  private:

    sf::Vector2f position_;
    std::array<sf::RectangleShape, 6> points_;
  	

  //virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

  public:
    Points();
    virtual void Draw(sf::RenderWindow& canvas) const;
    void update(int r_goals, int l_goals); 
    
    
    
   
    
    
    
  

    

    
    

    



};
};


#endif