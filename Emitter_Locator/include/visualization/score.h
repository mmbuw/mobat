#ifndef SCORE_H
#define SCORE_H

#include <glm/vec2.hpp>
#include <SFML/Graphics.hpp>
#include "Drawable_Object.h"



namespace TTT {

class Score : Drawable_Object{
  private:

    std::array<sf::RectangleShape, 6> points_;
    sf::Color color_red_;
  	sf::Color color_blue_;
  //virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

  public:
    Score(){};
    ~Score(){};
    Score(glm::vec2 const& min, glm::vec2 const& max);
    void Draw(sf::RenderWindow& canvas) const;
    void update(int r_goals, int l_goals); //changes colors according to given scores
    void reset();
};

};//Namespace TTT

#endif