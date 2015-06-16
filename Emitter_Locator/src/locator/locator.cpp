#include "locator.hpp"

#include <limits>
#include <iostream>

#include <numeric>
#include <thread>

Locator::Locator(unsigned int num_mics):
 shutdown{false},
 position{0, 0},
 cached_position{0, 0},
 window_size{512},
 recorder{num_mics, 44100, 300000},
 collector{recorder.buffer_bytes() / num_mics, num_mics},
 fft_transformer{window_size},
 locator{15000, {0.02, 0.02}, {0.02, 0.47}, {0.65, 0.47}, {0.665, 0.015}}
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
    // start recording loop
    auto recording_thread = std::thread{&Recorder::recording_loop, &recorder};

    while (!shutdown)
    {
        std::array<unsigned, 4> recognized_sample_pos = {0, 0, 0, 0};

        if(!recorder.new_recording()) {
            continue;
        }

        collector.from_interleaved(recorder.buffer());

        recorder.request_recording();

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
        unsigned max_sample = 0;

        double temp_avg = 0.0;
        //double average = std::accumulate(recognized_sample_pos.begin(), recognized_sample_pos.end(), 0) / recognized_sample_pos.size();

        unsigned int num_valid_entries = 0;

        for(unsigned channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

            unsigned current_channel_sample_pos = recognized_sample_pos[channel_iterator];

            if( current_channel_sample_pos != 0 ) { 
                if( current_channel_sample_pos < min_sample ) {
                 min_sample = current_channel_sample_pos;
                }

                if( current_channel_sample_pos > max_sample ) {
                    max_sample = current_channel_sample_pos;
                }

                temp_avg += current_channel_sample_pos;
                ++num_valid_entries;
            }

        }

        double average = temp_avg / num_valid_entries;

        double temp_sd = 0.0;

        for(unsigned channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {
            unsigned current_channel_sample_pos = recognized_sample_pos[channel_iterator];

            if( current_channel_sample_pos != 0 ) { 
                temp_sd += std::pow(average-current_channel_sample_pos, 2);
            }
        }

        double standard_deviation =   std::sqrt(temp_sd/num_valid_entries);
        std::cout << "standard_deviation: " << standard_deviation << "\n";

        if( max_sample-min_sample < 2000 ) {
            double updated_times[4];

            std::cout << "Sample diffs: ";

            for(unsigned channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {
                std::cout << "Channel " << channel_iterator << ": " << recognized_sample_pos[channel_iterator] - min_sample << "\n";

                if( std::abs(average - recognized_sample_pos[channel_iterator]) < std::abs(average -standard_deviation) )
                    updated_times[channel_iterator] = (recognized_sample_pos[channel_iterator] - min_sample) / 44100.0;
                else
                    updated_times[channel_iterator] = std::numeric_limits<unsigned>::max();
            }    


            //locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);
            locator.update_times(updated_times[0], updated_times[1], updated_times[2], updated_times[3]);



        std::cout << "Done.\n";
        cached_position = locator.locate();

        std::cout << "Cached position: " << cached_position.x << ", " << cached_position.y << "\n";

       }

        position_mutex.lock();
        position = cached_position;
        position_mutex.unlock();
    }
    // stop recording loop
    recorder.shutdown();
    recording_thread.join();
 }

 void Locator::shut_down() {
    shutdown = true;
 }