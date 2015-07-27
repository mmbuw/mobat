#ifndef SCORE_H
#define SCORE_H

#include <glm/vec2.hpp>
#include <SFML/Graphics.hpp>
#include "drawable_object.h"



namespace TTT {

class Score : DrawableObject{
  private:

    std::vector<sf::RectangleShape> points_;
    sf::Color color_red_;
  	sf::Color color_blue_;
    int maxpoints_;
  //virtual void Recalculate_Geometry(sf::Vector2f const& resolution) = 0;

  public:
    Score(){};
    ~Score(){};
    Score(int maxpoints);
    void Draw(sf::RenderWindow& canvas) const;
    void update(int r_goals, int l_goals); //changes colors according to given scores
    void reset();
    int get_maxpoints();
};

};//Namespace TTT

#endif