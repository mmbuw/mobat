#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>

class Test{
  private:
    std::ofstream myfile_;
    std::string file_;
    std::string entry_;

  public:
    Test();
    Test(std::string const& file);
    void update(unsigned const& freq, glm::vec2 const& pos);
    void write();
	
};
