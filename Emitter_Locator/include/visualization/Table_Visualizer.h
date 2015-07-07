#ifndef TABLE_VISUALIZER_H
#define TABLE_VISUALIZER_H

#include "Table_Object.h"
#include "Microphone_Object.h"
#include "Recognized_Token_Object.h"
// #include "Ball.h"
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
		bool gamemode_;

		glm::vec2 ball_pos_;
		// Ball ball_;	
		sf::CircleShape ball_;
		glm::vec2 ball_dir_;

		float ball_speed_min_;
		float ball_speed_max_;
		float ball_speed_;
		float ball_acceleration_;
		bool move_ball_;
		bool ball_reset_;
		int right_goals_;
		int left_goals_;

		Score points_;

		unsigned elapsed_milliseconds_since_last_frame_;

		void Calculate_Elapsed_Milliseconds();

		sf::RectangleShape projection_shape_;
	public:
		Table_Visualizer(std::vector<glm::vec2> const& microphone_positions
						 	= std::vector<glm::vec2>(),
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

		void Signal_Token(unsigned int in_id, glm::vec2 const& in_position);

		void update_tokens();


		unsigned Get_Elapsed_Milliseconds();

		std::pair<bool, glm::vec2> ball_intersect(Recognized_Token_Object const& paddle) const;

		void move_ball_out_of_token(Recognized_Token_Object const& paddle);

		std::pair<bool, std::string> game_over();

		void restart();

		void change_gm();

		bool wanna_play();
};

}; //end TTT
#endif