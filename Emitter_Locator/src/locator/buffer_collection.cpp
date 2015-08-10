#include "buffer_collection.hpp"
#include <stdexcept>
#include <cstring>

buffer_collection::buffer_collection()
 :count{0}
 ,bytes{0}
 ,max_bytes{0}
 ,length{0}
 ,max_length{0}
 ,buffers{nullptr}
{}

buffer_collection::buffer_collection(std::size_t bufferbytes, std::size_t buffernum)
 :count{buffernum}
 ,bytes{bufferbytes}
 ,max_bytes{bufferbytes}
 ,length{bufferbytes / 4}
 ,max_length{bufferbytes / 4}
 ,buffers{nullptr}
{
  buffers = (int32_t**)malloc(buffernum * sizeof(int32_t*));

  if (buffers) {
    for (std::size_t i = 0; i < count; ++i) {
      buffers[i] = (int32_t*)malloc(bufferbytes);
      if (!buffers[i]) {
        throw std::logic_error("memory allocation failed");
      }
    }
  }
  else throw std::logic_error("memory allocation failed");
}

buffer_collection::buffer_collection(buffer_collection const& coll)
 :buffer_collection{coll.max_bytes, coll.count}
{
  bytes = coll.bytes;
  length = coll.length;
  // copy over data
  for(std::size_t current_buffer = 0; current_buffer < count; ++current_buffer) {
    std::memcpy(buffers[current_buffer], coll.buffers[current_buffer], max_bytes);
  }
}

buffer_collection::buffer_collection(buffer_collection&& coll) {
  swap(*this, coll);
}

buffer_collection& buffer_collection::operator=(buffer_collection coll) {
  swap(*this, coll);
  return *this;
}

buffer_collection::~buffer_collection() {
  for (std::size_t i = 0; i < count; ++i) {
    if (buffers[i]) {
      free(buffers[i]);
      buffers[i] = nullptr;
    }
  }
  if (buffers) {
    free(buffers);
  }
}

//copy data from interleaved buffer, assumes that the sourcesize equals source_bytes * count 
void buffer_collection::fromInterleaved(uint8_t* source, std::size_t source_bytes) {
  if(source_bytes > max_bytes) {
    throw std::length_error("sourcesize bigger than size of buffers");
  }
  // set new sizes
  bytes = source_bytes;
  length = source_bytes / 4;

  fromInterleaved(source);
}

//copy data from interleaved buffer, assumes that the sourcesize equals bytes * count 
void buffer_collection::fromInterleaved(uint8_t* source) {
  // fill each channel
  for (std::size_t current_buffer = 0; current_buffer < count; ++current_buffer) {
    // and each int
    for (std::size_t buffer_offset_pos = 0; buffer_offset_pos < length; ++buffer_offset_pos) {

      buffers[current_buffer][buffer_offset_pos] 
      = 0x0 
      | source[buffer_offset_pos * (count*4)      + current_buffer * 4 ]  <<  0
      | source[buffer_offset_pos * (count*4) + 1  + current_buffer * 4 ]  <<  8
      | source[buffer_offset_pos * (count*4) + 2  + current_buffer * 4 ]  <<  16
      | source[buffer_offset_pos * (count*4) + 3  + current_buffer * 4 ]  <<  24;
    }
  }
}

int32_t* const& buffer_collection::operator[](std::size_t i) {
  return buffers[i];
}

int32_t* const& buffer_collection::operator[](std::size_t i) const {
  return buffers[i];
}

void swap(buffer_collection& c1, buffer_collection& c2) {
  std::swap(c1.count, c2.count);
  std::swap(c1.bytes, c2.bytes);
  std::swap(c1.max_bytes, c2.max_bytes);
  std::swap(c1.length, c2.length);
  std::swap(c1.max_length, c2.max_length);
  std::swap(c1.buffers, c2.buffers);
}
