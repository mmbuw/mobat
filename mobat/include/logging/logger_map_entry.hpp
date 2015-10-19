#ifndef MOBAT_LOGGER_MAP_ENTRY_PPP
#define MOBAT_TEST_MAP_ENTRY_PPP

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>


namespace MoB{

struct logger_map_entry{
  std::string filepath_;
  std::ofstream writer_;
  glm::vec2 average_position_;
  int num_entries_;

  logger_map_entry()
            : filepath_()
            , writer_()
            , average_position_()
            , num_entries_() {
  }

  logger_map_entry(std::string const& path)
  					: filepath_(path)
            , writer_(path)
            , average_position_()
            , num_entries_(0) {
  }

  logger_map_entry(logger_map_entry const& x)
            : filepath_(x.filepath_)
            , writer_(x.filepath_)
            , average_position_(x.average_position_)
            , num_entries_(x.num_entries_) {  
  }

  logger_map_entry operator=(logger_map_entry const& x) {
    filepath_ = x.filepath_;
    writer_.open(x.filepath_);
    average_position_ = x.average_position_;
    num_entries_ = x. num_entries_;
    return *this;
  }

};

}; // namespace MoB

#endif