#ifndef BUFFERCOLLECTION_HPP
#define BUFFERCOLLECTION_HPP

#include <cstdlib>
#include <vector>

struct buffer_collection {
    buffer_collection(std::size_t bufferbytes, std::size_t buffernum);

    //block implicit copy operations 
    buffer_collection(buffer_collection const&) = delete;
    buffer_collection& operator=(buffer_collection const&) = delete;

    ~buffer_collection();
    //copy data from interleaved buffer, assumes that the sourcesize equals length * count 
    void from_interleaved(unsigned char* source);

    int* const& operator[](std::size_t i);
    int* const& operator[](std::size_t i) const;
    // std::vector<int>& operator[](std::size_t i);
    // std::vector<int> const& operator[](std::size_t i) const;

    std::size_t count;
    // length in bytes
    std::size_t bytes;
    // length in ints
    std::size_t length;
    int** buffers;
    std::vector<std::vector<int>> buffers2;
};
#endif //BUFFERCOLLECTION_HPP
