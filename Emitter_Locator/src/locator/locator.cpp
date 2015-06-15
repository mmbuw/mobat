#include "locator.hpp"

#include <limits>
#include <iostream>

Locator::Locator(unsigned int num_mics):
 shutdown{false},
 position{0, 0},
 cached_position{0, 0},
 window_size{512},
 recorder{num_mics, 44100, 300000},
 collector{recorder.buffer_bytes() / num_mics, num_mics},
 fft_transformer{window_size},
 locator{100000, {2.0, 2.0}, {2.0, 47.0}, {46.5, 47.5}, {45.0, 3.0}}
 {
    fft_transformer.initialize_execution_plan();
    locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);
 }

 glm::vec2 Locator::load_position() const {
    // try to access current position
    if(position_mutex.try_lock()) {
        glm::vec2 temp = position;
        position_mutex.unlock();
        return temp;
    }
    // if recorder is writing, use cached one
    else {
        return cached_position;
    }
 }

 void Locator::record_position() {
    //unsigned int current_listened_channel = 0;



    while (!shutdown)
    {


        unsigned recognized_sample_pos[4] = {0, 0, 0, 0};

        recorder.record();

        collector.from_interleaved(recorder.buffer());

        for (unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {
            fft_transformer.set_FFT_buffers(collector.count, 
                    recorder.buffer_bytes()/collector.count,
                (int**)&collector[channel_iterator]);   


            fft_transformer.reset_sample_counters();
            fft_transformer.clear_cached_fft_results();
            for(unsigned int i = 0; i < 13000; ++i) {
                unsigned offset = 1 * i;
                if(offset > (12000) )
                    break;

                fft_transformer.set_analyzation_range(0+offset, window_size+50 + offset);
                

                unsigned int fft_result = fft_transformer.perform_FFT();

                if(fft_result == 1 ) {
                    std::cout << "Signal starts at sample @ channel " << channel_iterator << ": " << i << "\n";
                    recognized_sample_pos[channel_iterator] = i;
                    break;
                }

             
            } 
        }

        unsigned min_sample = std::numeric_limits<unsigned>::max();

        for(unsigned channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {
            if(recognized_sample_pos[channel_iterator] < min_sample) {
                min_sample = recognized_sample_pos[channel_iterator];
            }
        }


        double update_times[4];

        std::cout << "Sample diffs: ";

        for(unsigned channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {
            std::cout << "Channel " << channel_iterator << ": " << recognized_sample_pos[channel_iterator] - min_sample << "\n";
            update_times[channel_iterator] = (recognized_sample_pos[channel_iterator] - min_sample) / 44100.0;
        }    


        //locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);
        locator.update_times(update_times[0], update_times[1], update_times[2], update_times[3]);



        std::cout << "Done.\n";
        cached_position = locator.locate();

        std::cout << "Cached position: " << cached_position.x << ", " << cached_position.y << "\n";
        position_mutex.lock();
        position = cached_position;
        position_mutex.unlock();
    }
 }

 void Locator::shut_down() {
    shutdown = true;
 }
