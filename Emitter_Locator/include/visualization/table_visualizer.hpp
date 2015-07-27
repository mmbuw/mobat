#ifndef TableVisualizer_H
#define TableVisualizer_H

#include "Table_Object.h"
#include "microphone_object.h"
#include "Recognized_Token_Object.h"
// #include "Ball.h"
#include "score.h"

#include <vector>
#include <map>
#include <set>

#include <chrono>

namespace TTT {




class TableVisualizer : DrawableObject {

	private:
		Table_Object table_;
		std::vector<MicrophoneObject> microphones_;
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
		float ball_speed_limit_;
		float ball_speed_max_;
		float ball_speed_;
		float ball_acceleration_;
		bool move_ball_;
		bool ball_reset_;
		int right_goals_;
		int left_goals_;

		float player_speed_;
		bool player1_keyboard_;
		bool player2_keyboard_;
		glm::vec2 player1_pos_;
		glm::vec2 player2_pos_;

		bool moved_out_;
		glm::vec2 last_moved_dir_;

		std::chrono::high_resolution_clock::time_point time_of_last_goal_;
		unsigned ball_respawn_delay_;

		Score points_;

		unsigned elapsed_milliseconds_since_last_frame_;
		unsigned ball_slowing_time_;
		// keep track of contact times for ball deceleration
		std::chrono::high_resolution_clock::time_point  current_time_;
		std::chrono::high_resolution_clock::time_point  contact_time_;

		void Calculate_Elapsed_Milliseconds();

		sf::RectangleShape projection_shape_;
	public:
		TableVisualizer(std::vector<glm::vec2> const& microphone_positions
						 	= std::vector<glm::vec2>(),
						 sf::Color const& in_table_fill_color 
						 	= sf::Color::White,
						 sf::Color const& in_microphone_fill_color 
						 	= sf::Color(0,255,0, 125),
						 sf::Color const& in_token_fill_color 
						 	= sf::Color(255,0,0),
						 double ball_size = 25
						);
		~TableVisualizer();

		virtual void Draw(sf::RenderWindow& in_canvas) const;

		void Reset_Microphones(std::vector<MicrophoneObject> const& in_microphones);

		void recalculateGeometry();

		void Set_Canvas_Resolution(glm::vec2 const& in_resolution );

		void Set_Table_Fill_Color(sf::Color const& in_table_fill_color);

		void Set_Microphone_Fill_Color( sf::Color const& in_microphone_fill_color );

		void Set_Token_Fill_Color(unsigned frequency, sf::Color const& in_token_fill_color );

		void Set_Token_Recognition_Timeout( unsigned in_timeout_in_ms );

		void Signal_Token(unsigned int in_id, glm::vec2 const& in_position);

		void update_tokens();


		unsigned Get_Elapsed_Milliseconds();

		std::pair<bool, glm::vec2> ball_intersect(Recognized_Token_Object const& paddle) const;

		void move_ball_out_of_token(Recognized_Token_Object const& paddle, glm::vec2 const& dir = glm::vec2{0.0f});

		std::pair<bool, std::string> game_over();

		void restart();

		void change_gm();

		bool wanna_play();

		void handle_keyboard_input();

};

}; //end TTT
#endif