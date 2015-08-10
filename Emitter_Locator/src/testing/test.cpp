#include "test.h"


namespace TTT{

Test::Test()
  : files_(),
    file_(),
    entry_()
{}


void Test::update(unsigned const& freq, glm::vec2 const& pos){
  entry_ += "Token " + std::to_string(freq) + " at: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + "),";
  files_[std::to_string(freq)] << "(" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")\n";
}


void Test::openFiles(std::vector<std::pair<std::string, std::string>> const& names){
  for(auto const& name : names){
    files_[name.first].open(name.first +"/" + name.second);
  }
}


void Test::closeFiles(){
  for(auto & i : files_){
    i.second.close();
  }
  files_.clear();
}


};