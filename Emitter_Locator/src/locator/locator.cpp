#include "locator.hpp"

#include <limits>
#include <iostream>

#include <numeric>
#include <thread>
#include <algorithm>
#include <functional>


Locator::Locator(unsigned int num_mics):
 shutdown{false},
 position{0, 0},
 cached_position{0, 0},
 window_size{256},
 recorder{num_mics, 44100, 130000},
 collector{recorder.buffer_bytes() / num_mics, num_mics},
 fft_transformer{window_size},
 locator{330, {0.055, 0.08}, {0.95,  0.09}, {0.925,  1.92} , {0.105,  1.89}}
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

 glm::vec4 const Locator::load_toas() const {
        // try to access current times of arrival

    if(toas_mutex.try_lock()) {
        glm::vec4 temp = toas;
        //std::cout << "Im trylock if\n";
        toas_mutex.unlock();
        return temp;
    }
    else {
        return cached_toas;
    }
 } 

std::array<std::vector<unsigned int>,4> const Locator::load_signal_vis_samples() const {
    //try to access current signal vis samples

    if(signal_vis_samples_mutex.try_lock()) {
        std::array<std::vector<unsigned int>,4> temp;

        for(int i = 0; i < 4; ++i) {
            temp[i] = signal_vis_samples[i]; 
        }

        signal_vis_samples_mutex.unlock();
        return temp;
    } else {
        return cached_signal_vis_samples;
    }
}

std::array<unsigned, 4> const Locator::load_recognized_vis_sample_positions() const {
    if(recognized_vis_sample_pos_mutex.try_lock()) {
        std::array<unsigned ,4> temp;

        for(int i = 0; i < 4; ++i) {
            temp[i] = recognized_vis_sample_pos[i]; 
        }

        recognized_vis_sample_pos_mutex.unlock();
        return temp;
    } else {
        return cached_recognized_vis_sample_pos;
    }
}


#define CURRENT_NUM_RECORDED_MICS 4

 void Locator::record_position() {

    // start recording loop
    auto recording_thread = std::thread{&Recorder::recording_loop, &recorder};

    while (!shutdown)
    {

                //std::cout << "Started Record Position\n";
        std::array<unsigned, 4> recognized_sample_pos = {0, 0, 0, 0};
        std::array<unsigned, 4> signal_detected_at_sample = {100000, 100000, 100000, 100000};

        if(!recorder.new_recording()) {
            continue;
        }

      std::cout << "Cleared window\n";

        collector.from_interleaved(recorder.buffer());

        recorder.request_recording();

        for (unsigned int channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {
            fft_transformer.set_FFT_buffers(collector.count, 
                    recorder.buffer_bytes()/collector.count,
                (int**)&collector[channel_iterator]);   


            fft_transformer.reset_sample_counters(channel_iterator);
            fft_transformer.clear_cached_fft_results(channel_iterator);
            for(unsigned int i = 0; i < 1500; ++i) {
                unsigned offset = 1 * i;
                if(offset > (1400) )
                    break;

                fft_transformer.set_analyzation_range(0+offset, window_size+50 + offset);
                

                unsigned int fft_result = fft_transformer.perform_FFT(channel_iterator);

                if(fft_result == 1 ) {
                    std::cout << "Signal starts at sample @ channel " << channel_iterator << ": " << i << "\n";
                    //recognized_sample_pos[channel_iterator] = i;
                    break;
                }

             
            } 
        }

        unsigned min_sample = std::numeric_limits<unsigned>::max();
        unsigned max_sample = 0;

        double temp_avg = 0.0;
        //double average = std::accumulate(recognized_sample_pos.begin(), recognized_sample_pos.end(), 0) / recognized_sample_pos.size();

        unsigned int num_valid_entries = 0;


  
        for(unsigned channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {

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



/*
        for(unsigned channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {
            unsigned current_channel_sample_pos = recognized_sample_pos[channel_iterator];

            if( current_channel_sample_pos != 0 ) { 
                temp_sd += std::pow(average-current_channel_sample_pos, 2);
            }
        }
*/



        double updated_times[4];
        if( max_sample-min_sample < 3000 ) {


            std::cout << "Sample diffs: ";


            //locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);





        unsigned starting_sample_threshold =  50;



        unsigned signal_average[4] ;

        for(unsigned int channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {


            unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;
            unsigned sum = std::accumulate(fft_transformer.signal_results_[channel_iterator].begin(), 
                                           fft_transformer.signal_results_[channel_iterator].end(), 0, max);
            signal_average[channel_iterator] = sum ;/// fft_transformer.signal_results_[channel_iterator].size();

            unsigned num_pause_samples = 0;
            unsigned num_signal_samples = 0;

            unsigned sample_num = 0;

            unsigned peak = signal_average[channel_iterator];
            unsigned avg = std::accumulate(fft_transformer.signal_results_[channel_iterator].begin(), 
                                           fft_transformer.signal_results_[channel_iterator].end(), 0) /  fft_transformer.signal_results_[channel_iterator].size();

            bool allow_signal_detection = false;

                            //signal_detected_at_sample[channel_iterator] = 500;
            for(auto const& sig :fft_transformer.signal_results_[channel_iterator]) {

                if(sig < peak * 0.75) {
                    num_signal_samples = 0;

                    ++num_pause_samples;

                } else {


                    if(num_pause_samples > starting_sample_threshold) {
                        allow_signal_detection = true;
                    }

                    if(allow_signal_detection) {
                        ++num_signal_samples;



                        if( num_signal_samples > 300 ) {
                            //signal_detected_at_sample[channel_iterator] = sample_num;

                            //track backwards 'til the signal increases again

                           // unsigned current_val = sig;


                            for(int back_tracking_index = sample_num-1; back_tracking_index >= 0; --back_tracking_index) {
                                if(fft_transformer.signal_results_[channel_iterator][back_tracking_index] < avg ) {
                                    signal_detected_at_sample[channel_iterator] = back_tracking_index;
                                    break;
                                } else {
                                   // current_val = fft_transformer.signal_results_[channel_iterator][back_tracking_index];
                                }
                            }

                            break;
                        }
                    }


                }



                ++sample_num;
            }

        }

        unsigned int const & (*min) (unsigned int const &, unsigned int const &) = std::min<unsigned>;
        unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;

        unsigned sample_min = std::accumulate(signal_detected_at_sample.begin(),
                                              signal_detected_at_sample.end(),
                                              999999, min);
        unsigned sample_max = std::accumulate(signal_detected_at_sample.begin(),
                                              signal_detected_at_sample.end(),
                                              0, max);
        


        for(unsigned int mic_index = 0; mic_index < CURRENT_NUM_RECORDED_MICS; ++mic_index) {
            cached_toas[mic_index] = updated_times[mic_index];
        }

        toas_mutex.lock();
            for(unsigned int mic_index = 0; mic_index < CURRENT_NUM_RECORDED_MICS; ++mic_index) {
                toas[mic_index] = cached_toas[mic_index];
            }
            cached_toas.x = 4.0;

        toas_mutex.unlock();



            for(unsigned int signal_idx = 0; signal_idx < 4; ++signal_idx) {
                updated_times[signal_idx] = (signal_detected_at_sample[signal_idx] - sample_min) / 44.1000;
            }


        if(sample_max - sample_min < 500 && sample_max < 99999 ) {
            locator.update_times(updated_times[0], updated_times[1], updated_times[2], updated_times[3]);
            //locator.update_times(0.0,0.001,0.001,0.001);

            std::cout << "Done.\n";
            cached_position = locator.locate();
            cached_position.y = 1 - cached_position.y;
            std::cout << "Cached position: " << cached_position.x << ", " << cached_position.y << "\n";
            std::cout << "\n";

            std::cout << "\n";


            for(unsigned int sample_copy_index = 0; sample_copy_index < 4; ++sample_copy_index) {
                std::cout<< signal_detected_at_sample[sample_copy_index]  << "\n";
            }
        }




       }


            for(unsigned int sample_copy_index = 0; sample_copy_index < 4; ++sample_copy_index) {
                cached_signal_vis_samples[sample_copy_index] = fft_transformer.signal_results_[sample_copy_index];      
                cached_recognized_vis_sample_pos[sample_copy_index] = signal_detected_at_sample[sample_copy_index];
            }




        position_mutex.lock();
        position = cached_position;
        position_mutex.unlock();


        signal_vis_samples_mutex.lock();
        signal_vis_samples = cached_signal_vis_samples;
        signal_vis_samples_mutex.unlock();

        recognized_vis_sample_pos_mutex.lock();
        recognized_vis_sample_pos = cached_recognized_vis_sample_pos;
        recognized_vis_sample_pos_mutex.unlock();





    }
    // stop recording loop
    recorder.shutdown();
    recording_thread.join();
 }

 void Locator::shut_down() {
    shutdown = true;
 }
