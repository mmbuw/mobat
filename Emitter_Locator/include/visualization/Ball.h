#ifndef BALL_H
#define BALL_H

#include "Drawable_Object.h"
#include <glm/glm.hpp>
#include <iostream>

namespace TTT {




class Ball : Drawable_Object {
  private:
  	sf::CircleShape ball_;
  	sf::Vector2f dir_;

  public:
    Ball():
      ball_(0),
      dir_(0, 0)
      {}
    Ball(sf::Vector2f const& pos, double rad);

    virtual void Draw(sf::RenderWindow& canvas) const;

    sf::CircleShape get_Circle() const;
    void set_dir(sf::Vector2f const& dir);
    void Set_Fill_Color(sf::Color const& in_fill_color);
    void setPosition(double x, double y);
    sf::Vector2f getPosition() const;
    double getRadius() const;
  

    void give_new_Circle(sf::CircleShape const& c){ball_ = c;};
    //std::pair<bool, glm::vec2> circ_intersect(sf::CircleShape const& circle) const;

    //void get_out_of_da_circle(sf::CircleShape const& t);
};
};


#endif