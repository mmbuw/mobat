#ifndef TESTMAPENTRY_PPP
#define TESTMAPENTRY_PPP

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>


namespace TTT{

struct TestMapEntry{
  std::string filepath_;
  std::ofstream writer_;
  glm::vec2 average_position_;
  int num_entries_;

  TestMapEntry()
            : filepath_()
            , writer_()
            , average_position_()
            , num_entries_()
  {}

  TestMapEntry(std::string const& path)
  					: filepath_(path)
            , writer_(path)
            , average_position_()
            , num_entries_(0)
  {}

  TestMapEntry(TestMapEntry const& x)
            : filepath_(x.filepath_)
            , writer_(x.filepath_)
            , average_position_(x.average_position_)
            , num_entries_(x.num_entries_)
  {}

  TestMapEntry operator=(TestMapEntry const& x){
    filepath_ = x.filepath_;
    writer_.open(x.filepath_);
    average_position_ = x.average_position_;
    num_entries_ = x. num_entries_;
    return *this;
  }

};

};

#endif