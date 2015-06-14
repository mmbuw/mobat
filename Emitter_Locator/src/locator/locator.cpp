#include "locator.hpp"

#include <iostream>

Locator::Locator(unsigned int num_mics):
 position{0, 0},
 cached_position{0, 0},
 window_size{512},
 recorder{num_mics, 44100, 200000},
 collector{recorder.buffer_bytes() / num_mics, num_mics},
 fft_transformer{window_size},
 locator{100000, {0.0, 0.0}, {200, 0.0}, {0.0, 100.0}, {200.0, 100.0}}
 {
    fft_transformer.initialize_execution_plan();
    locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);
 }

 glm::vec2 Locator::load_position() const {
    // try to access current position
    if(position_mutex.try_lock()) {
        return position;
        position_mutex.unlock();
    }
    // if recorder is writing, use chached one
    else {
        return cached_position;
    }
 }

 void Locator::record_position() {
    unsigned int current_listened_channel = 0;

    while (true)
    {
        recorder.record();

        collector.from_interleaved(recorder.buffer());

        fft_transformer.set_FFT_buffers(collector.count, 
                recorder.buffer_bytes()/collector.count,
            (int**)&collector[current_listened_channel]);   

        for(unsigned int i = 0; i < 200000; ++i) {
            unsigned offset = 10 * i;
            if(offset > 200000)
                break;

            fft_transformer.set_analyzation_range(0+offset, window_size+50 + offset);
            

            if(fft_transformer.perform_FFT() ) {
                break;
            }

         
        } 

        cached_position = locator.locate();
        position_mutex.lock();
        position = cached_position;
        position_mutex.unlock();
    }
 }
