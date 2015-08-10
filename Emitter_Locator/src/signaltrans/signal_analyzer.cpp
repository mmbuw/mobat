#include "signal_analyzer.h"

#include "configurator.hpp"

#include <algorithm>
#include <functional>
#include <utility>
#include <chrono>

SignalAnalyzer::
SignalAnalyzer() : fft_window_size_( ({configurator().getUint("fft_window_size");  }) ),
                    fft_transformer_({fft_window_size_}),
                    max_sample_distance_(0) {
    fft_transformer_.initializeExecutionPlan();
    loadAnalyzerParameters();
}

SignalAnalyzer::
~SignalAnalyzer() {

}

void SignalAnalyzer::
loadAnalyzerParameters() {
  max_sample_distance_ = configurator().getUint("max_sample_distance");
}

void SignalAnalyzer::
analyze(buffer_collection const& collector, unsigned signal_chunk){
    std::chrono::high_resolution_clock::time_point start1 = std::chrono::high_resolution_clock::now();

    std::vector<unsigned> fft_result_frequencies;
    for(auto const& frequency_to_transform_entry : frequency_toas_mapping_) {
        /*
          filter frequencies that are below a reasonable 
          threshold in order to create some virtual space
          for special detections (like knock detecion)
        */
        if(frequency_to_transform_entry.first != 0)
        fft_result_frequencies.push_back(frequency_to_transform_entry.first);
    }

    /*
      forward the frequencies to the fft-transformer in order
      to prepare normalized buffers for these frequencies*/
    fft_transformer_.setListenedFrequencies(fft_result_frequencies);
    
    signal_detected_at_sample_per_frequency.clear();
    //std::cout << "before performing fft. " << "\n";  
    std::chrono::high_resolution_clock::time_point  start = std::chrono::high_resolution_clock::now();
    fft_transformer_.performFFTOnChannels(collector, fft_window_size_, signal_chunk);
    //std::cout << "after performing fft. " << "\n";  
    unsigned elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
                            (std::chrono::high_resolution_clock::now() - start).count(); 
    std::cout << "Time for FFT: " << elapsed_time << std::endl; 
    double updated_times[4];

   // unsigned starting_sample_threshold =  50;

    double signal_average[4];


    for(auto const& frequency_entry : frequency_toas_mapping_) {

        if(0 == frequency_entry.first) {
            signal_detected_at_sample_per_frequency[frequency_entry.first] = {100000, 100000, 100000, 100000};
            
            for(unsigned int channel_it = 0; channel_it < collector.count; ++channel_it) {

                for(unsigned int i = 0; i < collector.length; ++i ) {
                    if(collector[channel_it][i] > std::numeric_limits<int>::max()-1000) {
                        
                        // std::cout << "Channel " << channel_it << " detected at: " << i << "\n";
                        signal_detected_at_sample_per_frequency[frequency_entry.first][channel_it] = 500;
                        break;
                        //std::cout << current_audio_buffer[channel_it][i] << "\n";
                        //std::cout << "max: " << std::numeric_limits<int>::max() << "\n";
                    }
                }
            }
            // std::cout << "\n";

        } else {
            double const & (*d_max) (double const &, double const &) = std::max<double>;
            double const & (*d_min) (double const &, double const &) = std::min<double>;

            signal_detected_at_sample_per_frequency[frequency_entry.first] = {std::numeric_limits<unsigned>::max()};

            double power = configurator().getFloat("peak_power");
            double frequency_sum_detection_threshold = configurator().getFloat("frequency_sum_detection_threshold");

            for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

            unsigned sample_vector_size = fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].size();
			unsigned processed_samples = 0;





                double sum = std::accumulate(fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
                                               fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), 0.0, d_max);
                signal_average[channel_iterator] = sum ;/// fft_transformer.signal_results_[channel_iterator].size();


                unsigned sample_num = 0;

                double peak = signal_average[channel_iterator];

               
                double min = std::accumulate(fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
                                               fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), std::numeric_limits<double>::max(), d_min);

				

                sample_vector_size = fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator].size();
              	
              	processed_samples = 0;


                double powered_min  = pow(min,power);
                double powered_peak = pow(peak,power);

                for(auto& sig : fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator]) {
                	if( processed_samples >= (sample_vector_size - 1) ) {
                		break;
                	} else {



                        
                		if(peak-min > frequency_sum_detection_threshold) {
                			sig =  100.0 * (pow(sig,power) - powered_min) / (powered_peak - powered_min);

             
                		}
                		else {
                			sig = 0.0;
                		}
                	}
                	++processed_samples;
                }


                double normalized_peak = 100.0;


                double current_peak_to_push = 0.0;
                unsigned current_sample_to_push = 0;

               	peak_samples_per_frequency_[frequency_entry.first][channel_iterator].clear();

                for(auto const& sig : fft_transformer_.signal_results_per_frequency_[frequency_entry.first][channel_iterator]) {



                      if(sig > 0.90 * normalized_peak ) {

                      		if(current_peak_to_push < sig ) {
                      			current_peak_to_push = sig;
                      			current_sample_to_push = sample_num;
                      		} else {
	                      		//sum_increasing = false;
	                      	}
	                    
         

                      } else if(sig < 0.5 * normalized_peak) {
                      		if(current_sample_to_push != 0) {

 	                      		peak_samples_per_frequency_[frequency_entry.first][channel_iterator].push_back(current_sample_to_push);
	                      		//std::cout << "Channel " << channel_iterator << " pushed peak at position: " << current_sample_to_push << " with value " << current_peak_to_push << "\n";
	                      	}

	                      		current_peak_to_push = 0.0;
	                      		current_sample_to_push = 0;

                      }


                    ++sample_num;
                }



            }


        //use peaks for calculation
        {
            bool peaks_available_for_every_channel = true;


            std::array<std::vector<unsigned> ,4> const& current_frequency_peak_samples =
                peak_samples_per_frequency_[frequency_entry.first];

            // check if we found potential peaks for every channel, otherwise the localization would be unstable and
            // we do not perform it                
            for( unsigned channel_idx = 0; channel_idx < current_frequency_peak_samples.size(); ++channel_idx ) {
                if(peak_samples_per_frequency_[frequency_entry.first][channel_idx].size() == 0) {
                    peaks_available_for_every_channel = false;
                    break;
                }
            }


        	if( peaks_available_for_every_channel ) {

    			unsigned int const & (*min) (unsigned int const &, unsigned int const &) = std::min<unsigned>;
    			unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;


    			bool break_all_loops = false;

        		std::array<unsigned, 4> signal_one_to_four_pos;
        	  	for( unsigned channel_one_iterator = 0; channel_one_iterator < peak_samples_per_frequency_[frequency_entry.first][0].size(); ++channel_one_iterator) {
        			signal_one_to_four_pos[0] = peak_samples_per_frequency_[frequency_entry.first][0][channel_one_iterator];
        			for(unsigned channel_two_iterator = 0; channel_two_iterator < peak_samples_per_frequency_[frequency_entry.first][1].size(); ++channel_two_iterator) {
        				signal_one_to_four_pos[1]  = peak_samples_per_frequency_[frequency_entry.first][1][channel_two_iterator];
	         			for(unsigned channel_three_iterator = 0; channel_three_iterator < peak_samples_per_frequency_[frequency_entry.first][2].size(); ++channel_three_iterator) {
        					signal_one_to_four_pos[2]  = peak_samples_per_frequency_[frequency_entry.first][2][channel_three_iterator];
		 	         		for(unsigned channel_four_iterator = 0; channel_four_iterator < peak_samples_per_frequency_[frequency_entry.first][3].size(); ++channel_four_iterator) {
        						signal_one_to_four_pos[3]  = peak_samples_per_frequency_[frequency_entry.first][3][channel_three_iterator];

        						unsigned min_sample = std::accumulate(signal_one_to_four_pos.begin(), 
        															  signal_one_to_four_pos.end(),
        															  std::numeric_limits<unsigned>::max(),
        															  min);

         						unsigned max_sample = std::accumulate(signal_one_to_four_pos.begin(), 
        															  signal_one_to_four_pos.end(),
        															  0,
        															  max);


         						if(max_sample - min_sample < max_sample_distance_) {
						        	for(int i = 0; i < 4; ++i) {
							        	if( peak_samples_per_frequency_[frequency_entry.first][i].size() > 0 ) {
							        		//std::cout << peak_samples[i].size() << "\n";
						                    signal_detected_at_sample_per_frequency[frequency_entry.first][i] = signal_one_to_four_pos[i];
							        		vis_sample_pos_mapping_[frequency_entry.first][i] =  signal_one_to_four_pos[i];

							        		break_all_loops = true;

			
							        	}
						        	}
         						}
			        			if(break_all_loops) {
			        				break;
			        			}       	
		        			}
                            if(break_all_loops) {
		        				break;
		        			}       		       				
	        			}       	
	        			if(break_all_loops) {
		        			break;
		        		}     	
        			}
	        		if(break_all_loops) {
		        		break;
		        	}     	
        	  	}
        	}
        }
        }

    }



    unsigned int const & (*min) (unsigned int const &, unsigned int const &) = std::min<unsigned>;
    unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;
    
    for(auto const& frequency_entry : frequency_toas_mapping_) {

        unsigned sample_min = std::accumulate(signal_detected_at_sample_per_frequency[frequency_entry.first].begin(),
                                              signal_detected_at_sample_per_frequency[frequency_entry.first].end(),
                                              std::numeric_limits<unsigned>::max(), min);

        unsigned sample_max = std::accumulate(signal_detected_at_sample_per_frequency[frequency_entry.first].begin(),
                                                  signal_detected_at_sample_per_frequency[frequency_entry.first].end(),
                                                  0, max);

    
        if(sample_max-sample_min > 0 && sample_max - sample_min < max_sample_distance_ /*was 500*/ && sample_max < 999999) {

            for(unsigned int signal_idx = 0; signal_idx < 4; ++signal_idx) {
                updated_times[signal_idx] = (signal_detected_at_sample_per_frequency[frequency_entry.first][signal_idx] - sample_min) / 44100.0;
                frequency_toas_mapping_[frequency_entry.first][signal_idx] = updated_times[signal_idx];

                is_frequency_toa_mapping_valid_[frequency_entry.first] = true;
                
            }
        }
        else {
            is_frequency_toa_mapping_valid_[frequency_entry.first] = false;
        }
    }

    unsigned elapsed_time1 = std::chrono::duration_cast<std::chrono::milliseconds>
                            (std::chrono::high_resolution_clock::now() - start1).count(); 
    elapsed_time1 -=elapsed_time;
    std::cout << "Time for Analyzation: " << elapsed_time1 << std::endl; 

}

//frequencies we deregister are not taken into account in the analyzation step anymore
void SignalAnalyzer::
stopListeningTo(unsigned const frequency_to_stop_listening_to) {

    std::map<unsigned, std::array<double, 4> >::iterator frequency_iterator
        = frequency_toas_mapping_.find(frequency_to_stop_listening_to);


    if( frequency_toas_mapping_.end() != frequency_iterator ) {
        //delete the found entry:
        //we won't look for the corresponding toas anymore
        frequency_toas_mapping_.erase(frequency_iterator);
    }
}

//only frequencies we register are taken into account for the analyzation
void SignalAnalyzer::
startListeningTo(unsigned const frequency_to_start_listening_to) {
	//just overwrite the slot, even if there's already an entry for
	//the frequency
	frequency_toas_mapping_[frequency_to_start_listening_to]
		= std::array<double,4>();
}

std::array<double, 4> SignalAnalyzer::
getTOAsFor(unsigned const frequency) {


    std::map<unsigned, std::array<double, 4> >::const_iterator frequency_iterator
        = frequency_toas_mapping_.find(frequency);

        if( (frequency_toas_mapping_.end() != frequency_iterator) && (is_frequency_toa_mapping_valid_[frequency]) ) {

            if(frequency_iterator->second[0] != 0.0 
            || frequency_iterator->second[1] != 0.0 
            || frequency_iterator->second[2] != 0.0
            || frequency_iterator->second[3] != 0.0) {
                return frequency_iterator->second;
            } else {
                return {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()};
            }
        } 

        //TODO: add "Frequency not registered exception" instead of returning
        //      an empty array
        else {

            return {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()};
        }

}

std::map<unsigned, std::array<unsigned, 4> > const SignalAnalyzer::
getVisSamplePos() {
		return vis_sample_pos_mapping_;
}

std::map<unsigned, std::array<std::vector<double>,4> >const SignalAnalyzer::
getSignalSamples() {
    return fft_transformer_.signal_results_per_frequency_;
}

std::map<unsigned, std::array<std::vector<unsigned> ,4> > const SignalAnalyzer::
getRawPeakIndices() {
    return peak_samples_per_frequency_;
}