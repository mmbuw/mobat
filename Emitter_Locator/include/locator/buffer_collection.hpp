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

    int* const& operator[](std::size_t i) {
        return buffers[i];
    }

    std::size_t count;
    unsigned long length;
    int** buffers;
};
#endif //BUFFERCOLLECTION_HPP
