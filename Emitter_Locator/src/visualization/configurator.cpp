#include "configurator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

void Configurator::read(std::string const& filename) {
  std::ifstream file{filename};
  std::string line;
  while (std::getline(file, line)) {
    line.erase(remove_if(line.begin(), line.end(), isspace),line.end());
    std::istringstream line_stream{line};
    std::string name;
    std::getline(line_stream, name, ':');
    
    if (name.at(0) == '#') continue;

    std::size_t count = std::count(line.begin(), line.end(), ',');
    if (count == 0) {
      std::string val;
      std::getline(line_stream, val);
      bool is_uint = std::all_of(val.begin(), val.end(), isdigit);
      if (is_uint) {
        uints_[name] = atoi(val.c_str());
      }
      else {
       floats_[name] = atof(val.c_str());
      }
    }
    else if(count == 1) {
      std::string a, b;
      std::getline(line_stream, a, ',');
      std::getline(line_stream, b, ',');
      vecs_[name] = glm::vec2{atof(a.c_str()), atof(b.c_str())};
    }
    else {
      std::string element;
      std::vector<unsigned> list{};
      while(std::getline(line_stream, element, ',')) {
        list.push_back(atoi(element.c_str()));
      }

      lists_[name] = list;
    }
  }
}

void Configurator::print() const{
  std::cout << "floats" << std::endl;
  for (auto const& pair : floats_) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
  std::cout << "uints" << std::endl;
  for (auto const& pair : uints_) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
  std::cout << "vecs" << std::endl;
  for (auto const& pair : vecs_) {
    std::cout << pair.first << ": " << pair.second.x << ", "<< pair.second.y << std::endl;
  }
  std::cout << "lists" << std::endl;
  for (auto const& pair : lists_) {
    std::cout << pair.first << ": ";
    for( auto const& e : pair.second) {
      std::cout << e << ", ";
    }
    std::cout << std::endl;
  }

}

glm::vec2 const& Configurator::getVec(std::string const& name) const {
  if (vecs_.find(name) != vecs_.end()) {
    return vecs_.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return identity_;
}
float Configurator::getFloat(std::string const& name) const {
  if (floats_.find(name) != floats_.end()) {
    return floats_.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return 0.0f;
}
unsigned Configurator::getUint(std::string const& name) const {
  if (uints_.find(name) != uints_.end()) {
    return uints_.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return 0;
}
std::vector<unsigned> Configurator::getList(std::string const& name) const {
  if (lists_.find(name) != lists_.end()) {
    return lists_.at(name);
  }
  else throw std::out_of_range("key \'" + name + "\'' not found");
 
  return std::vector<unsigned>{};
}

Configurator& Configurator::inst() {
  static Configurator instance{};

  return instance;
}

Configurator& configurator() {
  return Configurator::inst();
}
