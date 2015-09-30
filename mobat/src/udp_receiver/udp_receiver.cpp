#include "udp_receiver.hpp"

#define REMOVE_LOST_TOKENS 0
namespace TTT {

    UdpReceiver::UdpReceiver()
        : positions_()
        ,recognized_tokens_()
        , last_time_stamp_()
    {}

    UdpReceiver::~UdpReceiver() {}
    
    void UdpReceiver::receive(std::pair<unsigned, glm::vec2> const& entry){
    	Token token{entry.first, entry.second};
    	positions_[entry.first] = token;
    	recognized_tokens_.insert(std::pair<unsigned, Token>{entry.first, token});
    }

    void UdpReceiver::printAll() const{
    	for(auto i : positions_){
    		std::cout<< i.first << " at: (" << i.second.physical_position_.x << ", " << i.second.physical_position_.y << ")\n";
    	}
    }
	
	void UdpReceiver::print(unsigned const& ID) const{
		std::cout<< ID << " at: (" << positions_.at(ID).physical_position_.x << ", " << positions_.at(ID).physical_position_.y << ")\n";
	}
	
	glm::vec2 UdpReceiver::positionOf(unsigned const& ID) const{
		return positions_.at(ID).physical_position_;
	}

	void UdpReceiver::draw(sf::RenderWindow& canvas){
		for(auto & i : positions_){
			i.second.recalculateGeometry();
			i.second.draw(canvas);
		}
	}

	glm::vec2 UdpReceiver::directionFromTo(unsigned ID1, unsigned ID2) const{
		return glm::normalize(glm::vec2{positions_.at(ID2).physical_position_.x - positions_.at(ID1).physical_position_.x
							, positions_.at(ID2).physical_position_.y - positions_.at(ID1).physical_position_.y});
	}

	void UdpReceiver::updateTokens(){
	   	//current time in milliseconds
	   	auto time_stamp =  std::chrono::high_resolution_clock::now();
		auto time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_stamp - last_time_stamp_);
	   	last_time_stamp_ = time_stamp;
	   	auto elapsed_milliseconds = time_in_ms.count();

    	std::set<unsigned> to_remove;
		
		for( auto& id_token_pair : positions_ ) {

		    bool refresh_token = false;
		    if(recognized_tokens_.end() != 
		       recognized_tokens_.find(id_token_pair.first) ) {
		        refresh_token = true;
		    }


		    
		    if( ! id_token_pair.second
		        .update(refresh_token, 
		                      elapsed_milliseconds, 
		                      id_token_pair.second.physical_position_) ) {
		        to_remove.insert(id_token_pair.first);
		    }

		}

		for(auto const& token_id : to_remove ) {
		    positions_.erase(token_id);
		}

		recognized_tokens_.clear();
	}

    

}; //namespace TTT

