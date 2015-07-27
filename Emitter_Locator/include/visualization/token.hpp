#ifndef TTT_Token_H
#define TTT_Token_H

#include "drawable_object.h"

namespace TTT {

class Token : DrawableObject {

	private:
		//the id is in this case the frequency
		unsigned id_;


		sf::CircleShape token_circle_shape_;

		//how long the object will life before we discard it


		//helper functions
		void Update_Alpha_Value(float in_alpha) ;

	public:
		unsigned life_time_in_ms_;
		unsigned remaining_life_time_in_ms_;

		glm::vec2 physical_position_;
		
		Token(unsigned in_id = 0, 
								glm::vec2 in_position = glm::vec2(-500.0, -500.0),
								unsigned in_life_time_in_ms = 10000);
		virtual void Draw(sf::RenderWindow& canvas) const;
		virtual void recalculateGeometry();

		void setFillColor(sf::Color const& in_fill_color);
		void Set_Life_Time(unsigned in_life_time_in_ms = 10000);

		//update remaining_life_time_ and physical_position_
		//returns "true", if the object is still alive
		//returns "false", if the object shall be deleted
		bool 
		Update_Token(bool in_was_recognized, 
					 unsigned in_passed_time_in_ms, 
					 glm::vec2 const& in_position 
					 	= glm::vec2(0.0f,0.0f));

		unsigned Get_Id() const;

		sf::CircleShape get_Circle() const;
		glm::vec2 const& get_physical_position() const;

}; //end class Token


}; //namespace TTT
#endif 
