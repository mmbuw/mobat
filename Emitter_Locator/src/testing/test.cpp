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
    //must be closed before reading from it
    i.second.writer_.close();

    auto standard_deviation = calculateStandardDeviation(i.second.filepath_, avg_pos, i.second.num_entries_);
    auto percentile = calculatePercentile(i.second.filepath_, avg_pos, standard_deviation);
    std::ofstream tmp(i.second.filepath_, std::ofstream::app);
    tmp << "standard deviation: " << standard_deviation << "\n";
    tmp << "90 percentile: " << percentile << "\n";
    tmp.close();
  }
  files_.clear();
}

double Test::calculateStandardDeviation(std::string const& path, glm::vec2 const& avg_pos, int num_entries){
  double std_drvtn = 0.0;
  std::ifstream file(path);
  std::string line;
  while(std::getline(file, line)){
    std::stringstream ss(line);
    std::string s;
    ss >> s;
    if('(' == s.at(0)){
      boost::erase_all(s, "(");
      boost::erase_all(s, ")");
      boost::erase_all(s, ",");
      double x = std::stod (s);
      ss >> s;
      double y = std::stod (s);
      std_drvtn += sqrt( pow ((x - avg_pos.x) * 100.0, 2)
                       + pow((y - avg_pos.y) * 100.0, 2));

    }
  }
  return std_drvtn / num_entries;
}

double Test::calculatePercentile(std::string const& path, glm::vec2 const& avg_pos, long){
  double perc = 0;
  std::vector<double> deviations;
  std::ifstream file(path);
  std::string line;
  while(std::getline(file, line)){
    std::stringstream ss(line);
    std::string s;
    ss >> s;
    if('(' == s.at(0)){
      boost::erase_all(s, "(");
      boost::erase_all(s, ")");
      boost::erase_all(s, ",");
      double x = std::stod (s);
      ss >> s;
      double y = std::stod (s);
      double tmp = sqrt( pow ((x - avg_pos.x) * 100.0, 2)
                       + pow((y - avg_pos.y) * 100.0, 2));
      deviations.push_back( tmp );
    }
  }

  std::sort(deviations.begin(), deviations.end());

  unsigned stop_after_element_num = deviations.size() * 0.9;

  unsigned element_idx = 0;
  for(auto const& deviation : deviations) {
    if(++element_idx == stop_after_element_num) {
      perc = deviation;
    }
  }

  return perc;
}

};