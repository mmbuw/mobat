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
class Address {
 public:
  virtual sockaddr const* base_address() const = 0;
  virtual sockaddr* base_address() = 0;
  virtual socklen_t bytes() const = 0;
};

class Address_ip4 : public Address {
 public:
  Address_ip4()
   :address_{}
  {}

  Address_ip4(std::uint32_t ip, std::uint16_t p)
   :address_{}
  {
    address_.sin_family = AF_INET;
    set_address(ip);
    set_port(p);
  }

  Address_ip4(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d, std::uint16_t p)
   :Address_ip4((std::uint32_t)((a << 24  | (b << 16) | (c << 8) | d)), p)
  {};

  std::uint32_t address() const {
    return ntohl(address_.sin_addr.s_addr);
  }

  void set_address(std::uint32_t ip4) {
    address_.sin_addr.s_addr = htonl(ip4);
  }

  std::uint16_t port() const {
    return ntohs(address_.sin_port);
  }

  void set_port(std::uint16_t port) {
    address_.sin_port = htons(port);
  }

  sockaddr const* base_address() const {
    return (sockaddr*)&address_;
  }
  sockaddr* base_address() {
    return (sockaddr*)&address_;
  }

  socklen_t bytes() const {
    return sizeof(address_);
  }

 private:
  sockaddr_in address_;
};

#endif