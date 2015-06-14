#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include <mutex>

#include "microphone.hpp"
#include "recorder.hpp"
#include "tdoator.hpp"
#include "FFT_Transformer.h"
#include "buffer_collection.hpp"

class Locator {
 public:
    Locator(unsigned int num_mics);

     glm::vec2 load_position() const;

     void record_position();

private:

    mutable std::mutex position_mutex;
    glm::vec2 position;
    glm::vec2 cached_position;

    unsigned short window_size;

    Recorder recorder;
    buffer_collection collector;
    FFT_Transformer fft_transformer;
    TDOAtor locator;
 };

 #endif //LOCATOR_HPP