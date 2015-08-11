#include "test.h"


namespace TTT{

Test::Test()
  : files_()
{}


void Test::update(unsigned const& freq, glm::vec2 const& pos){
  files_[std::to_string(freq)].writer_ << "(" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")\n";
  files_[std::to_string(freq)].average_position_ += pos;
  ++files_[std::to_string(freq)].num_entries_;
}


void Test::openFiles(std::vector<std::pair<std::string, std::string>> const& names){
  for(auto const& name : names){
    TTT::TestMapEntry tmp{name.first +"/" + name.second};
    files_[name.first] = tmp;
  }
}


void Test::closeFiles(){
  for(auto & i : files_){
    auto avg_pos = i.second.average_position_ / glm::vec2 {i.second.num_entries_, i.second.num_entries_};
    i.second.writer_ << "average position: (" << avg_pos.x << ", " << avg_pos.y << ")\n";
    auto standard_deviation = calculateStandardDeviation(i.second.filepath_, avg_pos);
    i.second.writer_ << "standard deviation: " << standard_deviation << "\n";
    i.second.writer_ << "20th percentile: " << calculatePercentile(i.second.filepath_, avg_pos, standard_deviation) << "\n";
    i.second.writer_.close();
  }
  files_.clear();
}

long Test::calculateStandardDeviation(std::string const& path, glm::vec2 const& avg_pos){
/*  long std_drvtn;
  std::ifstream file(path);
  std::string line;
  while(std::getline(file, line)){
    std::stringstream ss(line);
    std::string s;
    ss >> s;
    if(s.first == '('){
      std::cout << "WUHUWUHUWUHUWUHUWUHU\n";
    }
  }*/

  return 0;
}

int Test::calculatePercentile(std::string const& path, glm::vec2 const& avg_pos, long){

  return 0;
}

};