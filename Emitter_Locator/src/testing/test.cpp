#include "test.h"

Test::Test()
  : myfile_("positions.txt"),
    file_("positions.txt"),
    entry_()
{}

Test::Test(std::string const& file)
  : myfile_(file)
  , file_(file)
  , entry_()
{}

void Test::update(unsigned const& freq, glm::vec2 const& pos){
  entry_ += "Token " + std::to_string(freq) + " at: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + "),";

}

void Test::write(){
  entry_ += "\n";
  myfile_.open(file_, std::ofstream::app);
  myfile_ << entry_;
  myfile_.close();
  entry_ = "";
}