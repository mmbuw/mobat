#ifndef TOKEN_POSITION_HPP
#define TOKEN_POSITION_HPP

#include <cstdint>

struct token_position {
  token_position()
   :id{0}
   ,x{0}
   ,y{0}
  {}

  token_position(std::uint8_t i, float xx, float yy)
   :id{i}
   ,x{xx}
   ,y{yy}
  {}

  std::uint8_t id;
  float x;
  float y;
};

#endif