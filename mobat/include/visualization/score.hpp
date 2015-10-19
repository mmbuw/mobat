#ifndef SCORE_H
#define SCORE_H

#include <glm/vec2.hpp>
#include <SFML/Graphics.hpp>
#include "drawable_object.hpp"



namespace MoB {

class Score : DrawableObject{
  private:

    std::vector<sf::RectangleShape> points_;
    sf::Color color_red_;
  	sf::Color color_blue_;
    int max_points_;
  //virtual void recalculateGeometry(sf::Vector2f const& resolution) = 0;

  public:
    Score(){};
    ~Score(){};
    Score(int max_points);
    //draws
    void draw(sf::RenderWindow& canvas) const;
    //changes colors according to given scores
    void update(int r_goals, int l_goals);
    //sets colors back to the startcondition (like a tie)
    void reset();
    //returns maximum points
    int getMaxPoints();
};

};//Namespace MoB

#endif