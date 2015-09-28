#ifndef ADDRESS_HPP
#define ADDRESS_HPP

#include <cstdint>

#if defined(_WIN32)
  #include <winsock2.h>
  #pragma comment( lib, "wsock32.lib" )

  typedef int socklen_t;
#else
  #include <netinet/in.h>
#endif

// abstract address interface to make socket independet of types lie ip4/6
struct address {
  virtual sockaddr const* base_address() const = 0;
  virtual socklen_t bytes() const = 0;
};

struct address_ip4 : public address {
  address_ip4(std::uint32_t ip, std::uint16_t p)
   :ip4{ip}
   ,port{p}
  {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(ip4);
    address.sin_port = htons(port);
  }

  address_ip4(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d, std::uint16_t p)
   :address_ip4((std::uint32_t)((a << 24  | (b << 16) | (c << 8) | d)), p)
  {};

  sockaddr const* base_address() const {
    return (sockaddr*)&address;
  }

  socklen_t bytes() const {
    return sizeof(address);
  }

  sockaddr_in address;
  std::uint32_t ip4;
  std::uint16_t port;
};

#endif