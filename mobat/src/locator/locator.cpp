#include "locator.hpp"
#include "configurator.hpp"

#include "microphone.hpp"

#include <limits>
#include <iostream>

#include <numeric>
#include <thread>

#define SHOW_TIMES

Locator::Locator(unsigned int num_mics)
 :shutdown_{false}
 ,recorder_{num_mics, (unsigned)(configurator().getFloat("audio_device_sampling_rate")), configurator().getUint("max_recording_time")}
 ,collector_{recorder_.bufferBytes() / num_mics, num_mics}
 ,tdoator_{330.0f, configurator().getVec("microphone1_pos"),
                configurator().getVec("microphone2_pos"),
                configurator().getVec("microphone3_pos"),
                configurator().getVec("microphone4_pos")}
 ,locator_frame_counter_(cached_positions[0].size())
 ,current_signal_chunk_(0)
{
  loadLocatorParameters();
  for (auto freq : configurator().getList("known_frequencies")) {
    cached_positions[freq] = std::vector<std::pair<unsigned, glm::vec2>>(std::size_t{configurator().getUint("num_median_samples")});
  }
  one_euro_filter_ = configurator().getUint("one_euro_filter") > 0;
}

void Locator::
loadLocatorParameters() {
  request_vis_vectors_ = configurator().getUint("show_signalvis") > 0;
}

std::map<unsigned, std::pair<unsigned, glm::vec2> > Locator::
loadPosition() const {
  // try to access current position
  if (position_mutex.try_lock()) {
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
::loadTOAs() const {
  // try to access current times of arrival
  if (toas_mutex.try_lock()) {
    glm::vec4 temp = toas_;
    toas_mutex.unlock();
    return temp;
  }
  else {
    return cached_toas_;
  }
} 

std::map<unsigned, std::array<std::vector<double>,4> > const Locator::
loadSignalVisSamples() const {
  //try to access current signal vis samples

  if (signal_vis_samples_mutex.try_lock()) {
    std::map<unsigned, std::array<std::vector<double>,4> >temp;

    temp = signal_vis_samples; 
    
    signal_vis_samples_mutex.unlock();
    return temp;
  } else {
    return cached_signal_vis_samples;
  }
}

std::map<unsigned, std::array<unsigned, 4> > const Locator::
loadRecognizedVisSamplePositions() const {
  if (recognized_vis_sample_pos_mutex.try_lock()) {
    std::map<unsigned, std::array<unsigned ,4> > temp;

    temp = recognized_vis_sample_pos; 
    

    recognized_vis_sample_pos_mutex.unlock();
    return temp;
  } else {
    return cached_recognized_vis_sample_pos;
  }
}

std::map<unsigned, std::array<std::vector<unsigned> ,4> > const Locator::
loadPeakSamples() const {
  if (peak_sample_indices_mutex.try_lock()) {
    std::map<unsigned, std::array<std::vector<unsigned> ,4> > temp;

      temp = peak_sample_indices_; 
    

    peak_sample_indices_mutex.unlock();
    return temp;
  } else {
    return cached_peak_sample_indices_;
  }
}



 void Locator::setFrequenciesToRecord(std::vector<unsigned> const& frequencies_to_find) {

  if (frequency_to_record_setter_mutex.try_lock()) {
    frequencies_to_locate = frequencies_to_find;
    frequency_to_record_setter_mutex.unlock();
  }

 }

 void Locator::recordPosition() {


  bool show_times = configurator().getUint("show_times") > 0;
  std::chrono::high_resolution_clock::time_point start_loop, start;
  unsigned elapsed_time;

  // start recording loop
  auto recording_thread = std::thread{&Recorder::recordingLoop, &recorder_};

  unsigned num_chunks_to_analyze = configurator().getUint("num_analyzed_fourier_chunks");

  bool first_signal_available = false;
  while (!shutdown_) {

    if(show_times) {
      start_loop = std::chrono::high_resolution_clock::now();
    }

    for (unsigned frequency_to_analyze : frequencies_to_locate) {
      signal_analyzer_.startListeningTo(frequency_to_analyze);
    }

    bool work_on_old_signal = false; 
    if (!recorder_.newRecording()) {
      work_on_old_signal = true;
    } else {
      first_signal_available = true;
    }

    if (!work_on_old_signal) {
      current_signal_chunk_ = 0;
      // TODO: make recordedBytes() independent from buffer() call
      std::size_t buffer_bytes = recorder_.recordedBytes() / collector_.count;
      collector_.fromInterleaved(recorder_.buffer(), buffer_bytes);

      // recorder_.requestRecording();

    } else {
      if (!first_signal_available || ++current_signal_chunk_  >= num_chunks_to_analyze)
        continue;
    }

    if (collector_.length == 0) {
      continue;
    }
    // std::cout << "@ chunk: " << current_signal_chunk_+1 << "/" << num_chunks_to_analyze << "\n";

    signal_analyzer_.analyze(collector_, current_signal_chunk_); 

    if(show_times) {
      start = std::chrono::high_resolution_clock::now();
    }

    bool found_positions = false;

    std::map<unsigned,glm::vec2> currently_located_positions;

    for (unsigned frequency_to_locate : frequencies_to_locate) {

      std::array<double, 4> current_frequency_toas = signal_analyzer_.getTOAsFor(frequency_to_locate);

      double const & (*d_min) (double const &, double const &) = std::min<double>;
      double const & (*d_max) (double const &, double const &) = std::max<double>;

      double toa_min = std::accumulate(current_frequency_toas.begin(),
                                            current_frequency_toas.end(),
                                            std::numeric_limits<double>::max(), d_min);
      double toa_max = std::accumulate(current_frequency_toas.begin(),
                                       current_frequency_toas.end(),
                                       0.0, d_max);

      if (toa_max != std::numeric_limits<double>::max() && toa_max - toa_min < 100.00 ) {

        // std::cout << frequency_to_locate << std::endl;

        found_positions = true;

        currently_located_positions[frequency_to_locate] = tdoator_.locate(current_frequency_toas[0],
                                                                           current_frequency_toas[1],
                                                                           current_frequency_toas[2],
                                                                           current_frequency_toas[3]);
        currently_located_positions[frequency_to_locate].y = currently_located_positions[frequency_to_locate].y;
      }
    }
    //cached_signal_vis_samples = signal_analyzer_.get_signal_samples_for(17000);
    if(show_times) {
      unsigned elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now() - start).count(); 
      std::cout << "Time for TDOAtion: " << elapsed_time << std::endl;
      start = std::chrono::high_resolution_clock::now();
    }

    if (found_positions) {
      ++locator_frame_counter_;
      cached_located_positions.clear();

      for (auto const& currently_located_position_entry :  currently_located_positions) {
        glm::vec2 accumulated_position = glm::vec2(0.0, 0.0);
        //glm::temp_pos = glm::vec2(0.0, 0.0);
        cached_located_positions[currently_located_position_entry.first] = std::make_pair(  locator_frame_counter_ , currently_located_position_entry.second);
        if(cached_positions[currently_located_position_entry.first].size() != 0){
          cached_positions[currently_located_position_entry.first][locator_frame_counter_ % cached_positions[currently_located_position_entry.first].size()] = std::make_pair(locator_frame_counter_,currently_located_position_entry.second);
        }else{
          // cached_positions[currently_located_position_entry.first][locator_frame_counter_ % cached_positions[currently_located_position_entry.first].size()] = std::make_pair(locator_frame_counter_,currently_located_position_entry.second);
          continue;
        }


        if(one_euro_filter_){
          //one euro filter like mddling
        
          auto size_cached_positions = cached_positions[currently_located_position_entry.first].size();
          auto distance = glm::distance(cached_positions[currently_located_position_entry.first][size_cached_positions -2].second, cached_positions[currently_located_position_entry.first][size_cached_positions -1].second );
          unsigned alpha = ceil(size_cached_positions - distance);
          if(alpha < 1){
            alpha = 1;
          }else if(alpha >= size_cached_positions){
            alpha = size_cached_positions-1;
          }
          std::cout<<size_cached_positions<<   "      "  << alpha <<std::endl;
          for(unsigned i = size_cached_positions -1; i >= size_cached_positions - alpha; --i){
            accumulated_position += cached_positions[currently_located_position_entry.first][i].second;
          }
          //one euro like
          accumulated_position /= alpha;
        
        }else{

          //not one euro like middling
          unsigned normalization_counter = 0;
          std::vector< float > valid_x_pos;
          std::vector< float > valid_y_pos;



          for (auto const& glm_vec : cached_positions[currently_located_position_entry.first]) {
            if (locator_frame_counter_ - glm_vec.first < cached_positions[currently_located_position_entry.first].size()) {
                 valid_x_pos.push_back(glm_vec.second.x);
                 valid_y_pos.push_back(glm_vec.second.y);
                 
                 std::sort(valid_x_pos.begin(), valid_x_pos.end());
                 std::sort(valid_y_pos.begin(), valid_y_pos.end());

                 accumulated_position += glm_vec.second ;    
                 ++normalization_counter;                 
              //temp_pos = glm_vec * 2.0f;
              //accumulated_position += temp_pos;
            }
          }
          //float median_x = valid_x_pos[valid_x_pos.size()/2.0];
          //float median_y = valid_y_pos[valid_y_pos.size()/2.0];                

        
          accumulated_position /= normalization_counter;


          //accumulated_position.x = median_x;
          //accumulated_position.y = median_y;
        }

        cached_located_positions[currently_located_position_entry.first] = std::make_pair(  locator_frame_counter_ , accumulated_position);
      }

      position_mutex.lock();
      located_positions = cached_located_positions;

      position_mutex.unlock();
    }


    if(request_vis_vectors_) {
      cached_signal_vis_samples = signal_analyzer_.getSignalSamples();

      signal_vis_samples_mutex.lock();
      signal_vis_samples = cached_signal_vis_samples;
      signal_vis_samples_mutex.unlock();

      cached_recognized_vis_sample_pos = signal_analyzer_.getVisSamplePos();
      recognized_vis_sample_pos_mutex.lock();
      recognized_vis_sample_pos = cached_recognized_vis_sample_pos;
      recognized_vis_sample_pos_mutex.unlock();

      cached_peak_sample_indices_ = signal_analyzer_.getRawPeakIndices();
      peak_sample_indices_mutex.lock();
      peak_sample_indices_ = cached_peak_sample_indices_;
      peak_sample_indices_mutex.unlock();

      if(show_times) {
        elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
                  (std::chrono::high_resolution_clock::now() - start).count(); 
        std::cout << "Time for rest: " << elapsed_time << std::endl;

        elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
                  (std::chrono::high_resolution_clock::now() - start_loop).count(); 
        std::cout << "Time for Locator: " << elapsed_time << std::endl;
      }
    }
  }

  signal_analyzer_.shutdown();
  // stop recording loop
  recorder_.shutdown();
  recording_thread.join();
 }

 void Locator::shutdown() {
  shutdown_ = true;
 }
