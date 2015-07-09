#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include <glm/vec2.hpp>
#include <map>
#include <string>


// template<typename T>
// class Proxy {
//  public:
//   Proxy(T const* owner):
//    owner_{owner_}
//   {}

//   operator unsigned() {
//     return
//   }
//  private:
//   T const* owner_
// };

// config singleton
class Configurator {
 public:
  static Configurator& inst();
  void read(std::string const& filename);
  void print() const;

  glm::vec2 const& getVec(std::string const& name) const;
  float getFloat(std::string const& name) const;
  unsigned getUint(std::string const& name) const;

 private:
  Configurator(){};
  ~Configurator(){};
  Configurator(Configurator const&) = delete;
  Configurator& operator=(Configurator const&) = delete;

  std::map<std::string, glm::vec2> vecs_;
  std::map<std::string, float> floats_;
  std::map<std::string, unsigned> uints_;

  static glm::vec2 identity_;
};

Configurator& configurator();

#endif //CONFIGURATOR_HPP