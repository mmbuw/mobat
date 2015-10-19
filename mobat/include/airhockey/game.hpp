#ifndef GAME_H
#define GAME_H

#include "table.hpp"
#include "microphone.hpp"
#include "token.hpp"
#include "score.hpp"

#include <vector>
#include <map>
#include <set>

#include <chrono>

namespace MoB {


class Game : DrawableObject {

	private:
		std::vector<Microphone> microphones_;
		//register them with their frequency
		std::map<unsigned, Token> recognized_tokens_;

		std::map<unsigned, sf::Color> token_color_mapping_;

		unsigned token_recognition_timeout_;

		std::set<unsigned> tokens_to_refresh_;

		std::chrono::high_resolution_clock::time_point last_time_stamp_;

		//needed for pesudopong
		bool gamemode_;

		glm::vec2 ball_pos_;
		
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
		Table table_;
		
		Game(std::vector<glm::vec2> const& microphone_positions
						 	= std::vector<glm::vec2>(),
						 sf::Color const& in_table_fill_color 
						 	= sf::Color::White,
						 sf::Color const& in_microphone_fill_color 
						 	= sf::Color::Black,
						 sf::Color const& in_token_fill_color 
						 	= sf::Color(255,0,0),
						 double ball_size = 25
						);
		~Game();

		virtual void draw(sf::RenderWindow& in_canvas) const;
		//replaces mic vector
		void resetMicrophones(std::vector<Microphone> const& in_microphones);
		// actual game logic, calls recalculation of geometry for all tokens
		void recalculateGeometry();

		void setCanvasResolution(glm::vec2 const& in_resolution );

		void setTableFillColor(sf::Color const& in_table_fill_color);

		void setMicrophoneFillColor( sf::Color const& in_microphone_fill_color );

		void setTokenFillColor(unsigned frequency, sf::Color const& in_token_fill_color );

		void setTokenRecognitionTimeout( unsigned in_timeout_in_ms );

		void signalToken(unsigned int in_id, glm::vec2 const& in_position);
		//checks which tokens are recognized and which should die
		void updateTokens();

		//returns ellapsed milliseconds since last frame
		unsigned getElapsedMilliseconds();
		//checks for intersection between ball and given paddle
		std::pair<bool, glm::vec2> ballIntersect(Token const& paddle) const;
		// moves ball out of token if it gets inside; problem of realising a real situation in virtuality
		void moveBallOutOfToken(Token const& paddle, glm::vec2 const& dir = glm::vec2{0.0f});
		//returns bool if game is over and string with winner
		std::pair<bool, std::string> gameOver();
		// resets points and ball
		void restart();
		//turns game on and off
		void toggleGame();

		bool gameActive();
		//moves players according to keyboard input
		void handleKeyboardInput();
		//prints all acitve tokens
		void writeTokens();

};

}; //end TTT
#endif