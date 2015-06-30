#ifndef TABLE_VISUALIZER_H
#define TABLE_VISUALIZER_H

#include "Table_Object.h"
#include "Microphone_Object.h"
#include "Recognized_Token_Object.h"
#include "Ball.h"
#include "score.h"

#include <vector>
#include <map>
#include <set>

#include <chrono>

namespace TTT {




class Table_Visualizer : Drawable_Object {

	private:
		Table_Object table_;
		std::vector<Microphone_Object> microphones_;
		//register them with their frequency
		std::map<unsigned, Recognized_Token_Object> recognized_tokens_;

		std::map<unsigned, sf::Color> token_color_mapping_;

		unsigned token_recognition_timeout_;

		std::set<unsigned> tokens_to_refresh_;

		std::chrono::high_resolution_clock::time_point last_time_stamp_;

		//needed for pesudopong
		glm::vec2 ball_pos_;
		Ball ball_;	
		glm::vec2 ball_dir_;

		float ball_speed_;
		bool move_ball_;
		bool ball_reset_;
		int right_goals_;
		int left_goals_;


		Score points_;
		


		unsigned elapsed_milliseconds_since_last_frame_;

		void Calculate_Elapsed_Milliseconds();


	public:
		Table_Visualizer(glm::vec2 const& in_canvas_resolution = glm::vec2(800, 600),
						 glm::vec2 const& table_dims = glm::vec2(2.0,1.0), 
						 std::vector<sf::Vector2f> const& microphone_positions
						 	= std::vector<sf::Vector2f>(),
						 sf::Color const& in_table_fill_color 
						 	= sf::Color(82,159,153),
						 sf::Color const& in_microphone_fill_color 
						 	= sf::Color(0,255,0, 125),
						 sf::Color const& in_token_fill_color 
						 	= sf::Color(255,0,0),
						 double ball_size = 15
						);
		~Table_Visualizer();

		virtual void Draw(sf::RenderWindow& in_canvas) const;

		void Reset_Microphones(std::vector<Microphone_Object> const& in_microphones);

		void Recalculate_Geometry();

		void Set_Canvas_Resolution(glm::vec2 const& in_resolution );

		void Set_Table_Fill_Color(sf::Color const& in_table_fill_color);

		void Set_Microphone_Fill_Color( sf::Color const& in_microphone_fill_color );

		void Set_Token_Fill_Color(unsigned frequency, sf::Color const& in_token_fill_color );

		void Set_Token_Recognition_Timeout( unsigned in_timeout_in_ms );

		void Signal_Token(unsigned int in_id, sf::Vector2f const& in_position);

		void Finalize_Visualization_Frame();


		unsigned Get_Elapsed_Milliseconds();

		std::pair<bool, glm::vec2> circ_circ_intersect(sf::CircleShape const& ball, sf::CircleShape const& paddle) const;

		void move_ball_out_of_token(sf::CircleShape const& t);

		std::pair<bool, std::string> game_over();

		void restart();
};

}; //end TTT
#endif