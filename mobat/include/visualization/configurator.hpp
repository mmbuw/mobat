#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include <glm/vec2.hpp>
#include <map>
#include <vector>
#include <string>

// config singleton
class Configurator {
 public:
  // access the singleton instance
  static Configurator& inst();
  // load variables from file, can be calle dmutliple times
  void read(std::string const& filename);
  // print out all loaded variables
  void print() const;

  // return variable in given format, throws outofrange
  glm::vec2 const& getVec(std::string const& name) const;
  float getFloat(std::string const& name) const;
  unsigned getUint(std::string const& name) const;
  std::vector<unsigned> getList(std::string const& name) const;

 private:
  // prevent construction by user
  Configurator(){};
  ~Configurator(){};
  Configurator(Configurator const&) = delete;
  Configurator& operator=(Configurator const&) = delete;
  // loaded variables
  std::map<std::string, glm::vec2> vecs_;
  std::map<std::string, float> floats_;
  std::map<std::string, unsigned> uints_;
  std::map<std::string, std::vector<unsigned>> lists_; 
  // static var to return,when requested vec doesnt exist
  static glm::vec2 identity_;
};
// get singleton sinstance
Configurator& configurator();

#endif //CONFIGURATOR_HPP