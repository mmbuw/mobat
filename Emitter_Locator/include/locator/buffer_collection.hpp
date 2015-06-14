#ifndef BUFFERCOLLECTION_HPP
#define BUFFERCOLLECTION_HPP

struct buffer_collection {
    buffer_collection(unsigned long bufferlength, std::size_t buffernum) :
     count{buffernum},
     length{bufferlength}
    {
        buffers = (int**)malloc(buffernum * sizeof(int*));

        for(std::size_t i = 0; i < count; ++i) {
            buffers[i] = (int*)malloc(bufferlength);
        }
    }

    //block implicit copy operations 
    buffer_collection(buffer_collection const&) = delete;
    buffer_collection& operator=(buffer_collection const&) = delete;

    ~buffer_collection() {
        for(std::size_t i = 0; i < count; ++i) {
            free(buffers[i]);
            buffers[i] = nullptr;
        }
        free(buffers);
    }

    //copy data from interleaved buffer, assumes that the sourcesize equals length * count 
    void from_interleaved(unsigned char* source) {

        for(std::size_t current_buffer = 0; current_buffer < count; ++current_buffer) {

           for(std::size_t buffer_offset_pos = 0; buffer_offset_pos < length /4; ++buffer_offset_pos) {

                buffers[current_buffer][buffer_offset_pos] 
                    = 0x0 
                    | source[buffer_offset_pos * (count*4)      + current_buffer * 4 ]  << 0
                    | source[buffer_offset_pos * (count*4) + 1  + current_buffer * 4 ]  << 8
                    | source[buffer_offset_pos * (count*4) + 2  + current_buffer * 4 ]  << 16
                    | source[buffer_offset_pos * (count*4) + 3  + current_buffer * 4 ]  << 24;
            }
        }
    }

    int* const& operator[](std::size_t i) {
        return buffers[i];
    }

    std::size_t count;
    unsigned long length;
    int** buffers;
};
#endif //BUFFERCOLLECTION_HPP
