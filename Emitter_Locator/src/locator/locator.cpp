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
 locator{330, {0.055, 0.08}, {0.95,  0.09}, {0.105,  1.89}, {0.925,  1.92}},
 signal_plot_window_(sf::VideoMode(2400, 400)
                    , "Transformed_Frequencies")
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
        // try to access current position

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

#define CURRENT_NUM_RECORDED_MICS 4

 void Locator::record_position() {

    // start recording loop
    auto recording_thread = std::thread{&Recorder::recording_loop, &recorder};

    while (!shutdown)
    {

                //std::cout << "Started Record Position\n";
        std::array<unsigned, 4> recognized_sample_pos = {0, 0, 0, 0};


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
            for(unsigned int i = 0; i < 4000; ++i) {
                unsigned offset = 1 * i;
                if(offset > (3900) )
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

        //recognized_sample_pos = {500,0,500,500};
        /*
        recognized_sample_pos[0] = 500;
        recognized_sample_pos[1] = 0;
        recognized_sample_pos[2] = 500;
        recognized_sample_pos[3] = 500;
        */

  
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

        double average = temp_avg / num_valid_entries;

        double temp_sd = 0.0;



        for(unsigned channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {
            unsigned current_channel_sample_pos = recognized_sample_pos[channel_iterator];

            if( current_channel_sample_pos != 0 ) { 
                temp_sd += std::pow(average-current_channel_sample_pos, 2);
            }
        }

        double standard_deviation =   std::sqrt(temp_sd/num_valid_entries);
        std::cout << "standard_deviation: " << standard_deviation << "\n";


        std::cout << "Min sample: " << min_sample << "\n";
        if( max_sample-min_sample < 3000 ) {
            double updated_times[4];

            std::cout << "Sample diffs: ";



            for(unsigned channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {
                std::cout << "Channel " << channel_iterator << ": " << recognized_sample_pos[channel_iterator] - min_sample << "\n";

                if( std::abs(average - recognized_sample_pos[channel_iterator]) < std::abs(average -standard_deviation) )
                    updated_times[channel_iterator] = (recognized_sample_pos[channel_iterator] - min_sample) / 44100.0;
                else
                    updated_times[channel_iterator] = std::numeric_limits<unsigned>::max();
            }    


            //locator.update_times(0.0003062, 0.0012464, 0.0000, 0.0011279);
            locator.update_times(updated_times[0], updated_times[1], updated_times[2], updated_times[3]);
            
        //locator.update_times(0.00002267573, 0.0, 0.00002267573, 0.00002267573);
        //locator.update_times(0.0002267573, 0.0, 0.0002267573, 0.0002267573);
        std::cout << "Done.\n";
        cached_position = locator.locate();
        cached_position.y = 1 - cached_position.y;
        std::cout << "Cached position: " << cached_position.x << ", " << cached_position.y << "\n";

        signal_plot_window_.clear(sf::Color(255, 255, 255));


        unsigned starting_sample_threshold =  50;

        std::array<unsigned, 4> signal_detected_at_sample = {10000, 10000, 10000, 10000};

        unsigned signal_average[4] ;

        for(unsigned int channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {


            unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;
            unsigned sum = std::accumulate(fft_transformer.signal_results_[channel_iterator].begin(), 
                                           fft_transformer.signal_results_[channel_iterator].end(), 0, max);
            signal_average[channel_iterator] = sum ;/// fft_transformer.signal_results_[channel_iterator].size();

            unsigned num_pause_samples = 0;
            unsigned num_signal_samples = 0;

            unsigned sample_num = 0;

            unsigned avg = signal_average[channel_iterator];

            bool allow_signal_detection = false;

                            //signal_detected_at_sample[channel_iterator] = 500;
            for(auto const& sig :fft_transformer.signal_results_[channel_iterator]) {

                if(sig < avg * 0.75) {
                    num_signal_samples = 0;

                    ++num_pause_samples;

                } else {


                    if(num_pause_samples > starting_sample_threshold) {
                        allow_signal_detection = true;
                    }

                    if(allow_signal_detection) {
                        ++num_signal_samples;

                        //std::cout << "It.\n";
                        //std::cin.get();
                       /* if( num_signal_samples > 10) {
                            num_pause_samples = 0;
                        }*/

                        if( num_signal_samples > 600 ) {
                            signal_detected_at_sample[channel_iterator] = sample_num;
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
        
        if(sample_max - sample_min < 500 ) {
            std::cout << "!!!!\n";
            std::cout << "s0: " << signal_detected_at_sample[0] << "\n";
            std::cout << "s1: " << signal_detected_at_sample[1] << "\n";
            std::cout << "s2: " << signal_detected_at_sample[2] << "\n";
            std::cout << "s3: " << signal_detected_at_sample[3] << "\n";
            std::cin.get();
        }

        for(unsigned int channel_iterator = 0; channel_iterator < CURRENT_NUM_RECORDED_MICS; ++channel_iterator) {
            unsigned int sample_num = 0;

            unsigned avg = signal_average[channel_iterator];

            std::cout << "Avg: " << avg << "\n";
            std::cout << "Num Samples: " << fft_transformer.signal_results_[channel_iterator].size() << "\n";

                for(auto const& sig :fft_transformer.signal_results_[channel_iterator]) {



                    float width = 2400.0f / fft_transformer.signal_results_[channel_iterator].size();



                    sf::RectangleShape data_point(sf::Vector2f(1,sig) );
                    data_point.setPosition( sf::Vector2f( width * sample_num, channel_iterator * 100.0 + (100.0-sig) ) );

                    /*
                    if(sig < avg * 1.1 && sig > 3.0)
                        if(sample_num > starting_sample_threshold)
                            data_point.setFillColor(sf::Color(255, 0, 0) ) ;
                        else
                            data_point.setFillColor(sf::Color(0, 0, 0) ) ;
                    else
                        if(sample_num > starting_sample_threshold)
                            data_point.setFillColor(sf::Color(0, 255, 0) );
                        else
                            data_point.setFillColor(sf::Color(0, 0, 255) );
                    */

                       // std::cout << sample_num << " : " << signal_detected_at_sample[channel_iterator] <<  "\n";
                    if(sample_num < signal_detected_at_sample[channel_iterator]) {
                        data_point.setFillColor(sf::Color(255, 0, 0) ) ;
                    } else {
                        data_point.setFillColor(sf::Color(0, 255, 0) ) ;          
                    }
                    

                    signal_plot_window_.draw(data_point);

                   // std::cout << "Sig: " << sig << "\n";



                    ++sample_num;
                }
        }


        signal_plot_window_.display();

        //glm::vec4 update_times = {}
        //double updated_times[4] =  {4.3, 3.3, 2.3, 1.3};

        for(unsigned int mic_index = 0; mic_index < CURRENT_NUM_RECORDED_MICS; ++mic_index) {
            cached_toas[mic_index] = updated_times[mic_index];
        }

        toas_mutex.lock();
            for(unsigned int mic_index = 0; mic_index < CURRENT_NUM_RECORDED_MICS; ++mic_index) {
                toas[mic_index] = cached_toas[mic_index];
            }
            cached_toas.x = 4.0;

        toas_mutex.unlock();

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
