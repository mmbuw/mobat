#include "locator.hpp"

#include <limits>
#include <iostream>

#include <numeric>
#include <thread>


Locator::Locator(unsigned int num_mics):
 shutdown{false},
 recorder{num_mics, 44100, 130000},
 collector{recorder.buffer_bytes() / num_mics, num_mics},
 locator{330, {0.06, 0.075}, {0.945,  0.09}, {0.925,  1.915} , {0.06,  1.905}},
 locator_frame_counter_(1)
 {}


std::map<unsigned, std::pair<unsigned, glm::vec2> > Locator::
load_position() const {
    // try to access current position
    if(position_mutex.try_lock()) {
        std::map<unsigned, std::pair<unsigned, glm::vec2> > temp = located_positions;
        position_mutex.unlock();
        return temp;
    }
    // if recorder is writing, use cached one
    else {
        return cached_located_positions;
    }
}

glm::vec4 const Locator
::load_toas() const {
    // try to access current times of arrival
    if(toas_mutex.try_lock()) {
        glm::vec4 temp = toas;
        toas_mutex.unlock();
        return temp;
    }
    else {
        return cached_toas;
    }
} 

std::array<std::vector<double>,4> const Locator::
load_signal_vis_samples() const {
    //try to access current signal vis samples

    if(signal_vis_samples_mutex.try_lock()) {
        std::array<std::vector<double>,4> temp;

        for(int i = 0; i < 4; ++i) {
            temp[i] = signal_vis_samples[i]; 
        }

        signal_vis_samples_mutex.unlock();
        return temp;
    } else {
        return cached_signal_vis_samples;
    }
}

std::array<unsigned, 4> const Locator::
load_recognized_vis_sample_positions() const {
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



 void Locator::set_frequencies_to_record(std::vector<unsigned> const& frequencies_to_find) {

    if(frequency_to_record_setter_mutex.try_lock()) {
        frequencies_to_locate = frequencies_to_find;

        frequency_to_record_setter_mutex.unlock();
    }
 }

 void Locator::record_position() {

    // start recording loop
    auto recording_thread = std::thread{&Recorder::recording_loop, &recorder};

    while (!shutdown)
    {
        if(!recorder.new_recording()) {
            continue;
        }

        collector.from_interleaved(recorder.buffer());

        recorder.request_recording();

        for (unsigned frequency_to_analyze : frequencies_to_locate) {
            signal_analyzer.start_listening_to(frequency_to_analyze);
        }

        signal_analyzer.analyze(collector); 
        
        bool found_positions = false;

        std::map<unsigned,glm::vec2> currently_located_positions;

        for(unsigned frequency_to_locate : frequencies_to_locate) {

            std::array<double, 4> current_frequency_toas = signal_analyzer.get_toas_for(frequency_to_locate);

            double const & (*d_min) (double const &, double const &) = std::min<double>;
            double const & (*d_max) (double const &, double const &) = std::max<double>;

            double toa_min = std::accumulate(current_frequency_toas.begin(),
                                                  current_frequency_toas.end(),
                                                  std::numeric_limits<double>::max(), d_min);
            double toa_max = std::accumulate(current_frequency_toas.begin(),
                                             current_frequency_toas.end(),
                                             0.0, d_max);


            if(toa_max != std::numeric_limits<double>::max() && toa_max - toa_min < 100.00 ) {

                found_positions = true;


                locator.update_times(current_frequency_toas[0], current_frequency_toas[1], current_frequency_toas[2], current_frequency_toas[3]);

                currently_located_positions[frequency_to_locate] = locator.locate();
                currently_located_positions[frequency_to_locate].y = 1 - currently_located_positions[frequency_to_locate].y;
            }
        }

        cached_signal_vis_samples = signal_analyzer.get_signal_samples_for(18000);

        if(found_positions) {
            ++locator_frame_counter_;
            cached_located_positions.clear();
            for(auto const& currently_located_position_entry :  currently_located_positions) {
                cached_located_positions[currently_located_position_entry.first] = std::make_pair(  locator_frame_counter_ , currently_located_position_entry.second);
            }

            position_mutex.lock();
            located_positions = cached_located_positions;
            position_mutex.unlock();

        }


        signal_vis_samples_mutex.lock();
        signal_vis_samples = cached_signal_vis_samples;
        signal_vis_samples_mutex.unlock();


        cached_recognized_vis_sample_pos = signal_analyzer.get_vis_sample_pos_for(18000);
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
