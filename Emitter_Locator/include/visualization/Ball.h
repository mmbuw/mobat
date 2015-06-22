#ifndef BALL_H
#define BALL_H

#include "Drawable_Object.h"
#include <glm/glm.hpp>

namespace TTT {




class Ball : Drawable_Object {
  private:
  	sf::Vector2f position_;
  	double rad_;
  	sf::CircleShape ball_;
  	sf::Vector2f dir_;



	//virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

  public:
  	Ball(sf::Vector2f const& pos, double rad);
  	virtual void Draw(sf::RenderWindow& canvas) const;
  	virtual void Recalculate_Geometry();
  	sf::CircleShape get_Circle() const;
  	void set_dir(sf::Vector2f const& dir);



};
};


#endif