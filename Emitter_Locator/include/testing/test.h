#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vector>

class Test{
  private:
    std::map<std::string, std::ofstream> files_;
    std::string file_;
    std::string entry_;

  public:
    Test();
    void update(unsigned const& freq, glm::vec2 const& pos);
    void openFiles(std::vector<std::pair<std::string, std::string>> const& names);
    void closeFiles();
};
