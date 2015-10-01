#ifndef TTT_TEST_H
#define TTT_TEST_H


#include <fstream>

#include <sstream>
#include <map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <cmath>

#include "testMapEntry.hpp"

namespace TTT{

  
class Test{
  private:
    std::map<std::string, TTT::TestMapEntry> files_;
    //timestamp of current log session
    std::string timestamp_;

  public:
    Test();
    //writes positions to files
    void update(unsigned const& freq, glm::vec2 const& pos);
    //opens log files in given directory and names them, also sets timestamp, pair<frequency, timestamp>
    void openFiles(std::vector<std::pair<std::string, std::string>> const& names);
    //closes all open log files, calls calculation of standard deviation, 90percentile and positions per second
    void closeFiles();
    //calculates standard deviation
    double calculateStandardDeviation(std::string const& path, glm::vec2 const& avg_pos, int num_entries);
    //calculates 90 percentile of positions in given file
    double calculatePercentile(std::string const& path, glm::vec2 const& avg_pos, long standard_deviation);
    //returns timestamp of current log session
    std::string getTimestamp();
};

};


#endif