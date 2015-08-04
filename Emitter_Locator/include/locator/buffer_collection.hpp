#ifndef BUFFERCOLLECTION_HPP
#define BUFFERCOLLECTION_HPP

#include <cstdlib>
#include <cstdint>
#include <vector>

struct buffer_collection {
    buffer_collection(std::size_t bufferbytes, std::size_t buffernum);
    ~buffer_collection();

    //block implicit copy operations 
    buffer_collection(buffer_collection const&) = delete;
    buffer_collection& operator=(buffer_collection const&) = delete;

    //copy data from interleaved buffer, assumes that the sourcesize equals length * count 
    void fromInterleaved(uint8_t* source);
    // random access operators without index check
    int32_t* const& operator[](std::size_t i);
    int32_t* const& operator[](std::size_t i) const;
    
    // number of buffers
    std::size_t count;
    // length in bytes
    std::size_t bytes;
    // length in ints
    std::size_t length;
    int32_t** buffers;
};
#endif //BUFFERCOLLECTION_HPP
