#ifndef TOKEN_POSITION_HPP
#define TOKEN_POSITION_HPP

#include <cstdint>

struct token_position {
  token_position()
   :id{0}
   ,x{0.0f}
   ,y{0.0f}
   ,timestamp{0}
  {}

  token_position(std::uint32_t i, float xx, float yy, std::uint32_t ts)
   :id{i}
   ,x{xx}
   ,y{yy}
   ,timestamp{ts}
  {}

  std::uint32_t id;
  float x;
  float y;
  std::uint32_t timestamp;
};

#endif