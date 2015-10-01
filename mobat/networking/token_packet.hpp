#ifndef TOKEN_PACKET_HPP
#define TOKEN_PACKET_HPP

#include <cstdint>

struct token_packet {
  token_packet()
   :id{0}
   ,x{0.0f}
   ,y{0.0f}
   ,timestamp{0}
  {}

  token_packet(std::uint32_t i, float xx, float yy, std::uint32_t ts)
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

inline std::ostream& operator<<(std::ostream& out, const token_packet& tp){
   return out << "{ id: "       << tp.id
              << "; x: "        << tp.x
              << "; y: "        << tp.y
              << "; timestamp: "<< tp.timestamp 
              << " }\n";
}
#endif