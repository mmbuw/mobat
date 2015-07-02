#ifndef SCORE_H
#define SCORE_H

#include <glm/vec2.hpp>
#include <SFML/Graphics.hpp>

namespace TTT {

class Score{
  private:

    std::array<sf::RectangleShape, 6> points_;
  	
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