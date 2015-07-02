#include "Signal_Analyzer.h"


#include <algorithm>
#include <functional>
#include <utility>

Signal_Analyzer::
Signal_Analyzer() : fft_window_size(256),
                    fft_transformer{fft_window_size} {
    fft_transformer.initialize_execution_plan();

}

Signal_Analyzer::
~Signal_Analyzer() {

}

void Signal_Analyzer::
analyze(buffer_collection const& collector){

	std::vector<unsigned> fft_result_frequencies;
	for(auto const& frequency_to_transform_entry : frequency_toas_mapping) {
		/*
		  filter frequencies that are below a reasonable 
		  threshold in order to create some virtual space
		  for special detections (like knock detecion)
		*/
		if(frequency_to_transform_entry.first < 50000.0f)
		fft_result_frequencies.push_back(frequency_to_transform_entry.first);
    }

	/*
	  forward the frequencies to the fft-transformer in order
	  to prepare normalized buffers for these frequencies*/
	fft_transformer.set_listened_frequencies(fft_result_frequencies);
    
    signal_detected_at_sample_per_frequency.clear();

    fft_transformer.perform_FFT_on_channels((int**)&collector[0], collector.length, fft_window_size);

    double updated_times[4];

   // unsigned starting_sample_threshold =  50;

    double signal_average[4];

    for(auto const& frequency_entry : frequency_toas_mapping) {

        if(frequency_entry.first >= 50000) {

            signal_detected_at_sample_per_frequency[frequency_entry.first] = {100000, 100000, 100000, 100000};
            
            for(unsigned int channel_it = 0; channel_it < collector.count; ++channel_it) {
                for(unsigned int i = 0; i < collector.length; ++i ) {
                    if(collector[channel_it][i] > std::numeric_limits<int>::max()-1000 /* 2147000000*/) {
                        
                        // std::cout << "Channel " << channel_it << " detected at: " << i << "\n";
                        signal_detected_at_sample_per_frequency[frequency_entry.first][channel_it] = i;
                        break;
                        //std::cout << current_audio_buffer[channel_it][i] << "\n";
                        //std::cout << "max: " << std::numeric_limits<int>::max() << "\n";
                    }
                }
            }
            // std::cout << "\n";

        } else {


        	//double peaks[4];
        	//double mins[4];
        	//double ranges[4];
            double const & (*d_max) (double const &, double const &) = std::max<double>;
            double const & (*d_min) (double const &, double const &) = std::min<double>;

            signal_detected_at_sample_per_frequency[frequency_entry.first] = {100000, 100000, 100000, 100000};

            for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {

                double sum = std::accumulate(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
                                               fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), 0.0, d_max);
                signal_average[channel_iterator] = sum ;/// fft_transformer.signal_results_[channel_iterator].size();

               // unsigned num_pause_samples = 0;
                //unsigned num_signal_samples = 0;

                unsigned sample_num = 0;

                double peak = signal_average[channel_iterator];

                //peaks[channel_iterator] = peak;

               
                double min = std::accumulate(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
                                               fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), std::numeric_limits<double>::max(), d_min);

				
				//mins[channel_iterator] = min;
               	//double range = peak - min;

               //	ranges[channel_iterator] = range;
                
                /*
                double avg = std::accumulate(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
                                               fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), 0) 
                                                /  fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].size();

                bool allow_signal_detection = false;
*/

                unsigned sample_vector_size = fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].size();
              	
              	unsigned processed_samples = 0;

                for(auto& sig : fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator]) {
                	if( processed_samples >= (sample_vector_size - 1) ) {
                		break;
                	} else {
                		if(peak-min > 5.0) {
                			sig =  100.0 * (sig - min) / (peak - min);

                			if( sig < 40.0)
                				sig = 0; 
                		}
                		else {
                			sig = 0.0;
                		}
                	}
                	++processed_samples;
                }


                double normalized_peak = 100.0;
                //double normalized_min = 0.0;

                bool found_low_value = false;

                for(auto const& sig : fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator]) {

                		//std::cout << "signal: "<< sig << "   peak: " << peak << "\n";

/*
                    if(sig < peak * 0.75) {
                      //  num_signal_samples = 0;


                        //++num_pause_samples;
                    } else*/


                    //unsigned num_signal_samples = 0;
                    if( sample_num > 300 && sig > 0 &&  sig <= 0.99*normalized_peak ) {
                    	found_low_value = true;
                    }

                    if ( true == found_low_value ) {

                   		 unsigned num_signal_samples = 0;

                   		 if(sig >= 0.99*normalized_peak) {


  		                 		 

	                    	unsigned int apex_finder = sample_num;// + 200 ;
	                    	/*
	                    	for(apex_finder = sample_num ; apex_finder > 0; --apex_finder ) {
	                    		if( fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][apex_finder] < 0.2*normalized_peak )
	                    			break;
	                    	}
							*/
	                    	//apex_finder = sample_num;

	                    	//if(apex_finder > 700) {
	                        	signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] = apex_finder; //+ 1000;
	                        	vis_sample_pos_mapping[frequency_entry.first][channel_iterator] = apex_finder;// + 1000;
	          	
	                        //}

	                       break;
                       
                   		 	++num_signal_samples;
                   		 }

                        /*
                        if(num_pause_samples > starting_sample_threshold) {
                            allow_signal_detection = true;
                        }

                        if(allow_signal_detection) {
                            ++num_signal_samples;


                            if( num_signal_samples > 200 ) {

   

                                break;
                                
                                for(int back_tracking_index = sample_num-1; back_tracking_index >= 0; --back_tracking_index) {

                                    //if(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][back_tracking_index] < avg *0.50) {
                                	double max_value = 0.0;
                                	unsigned s_p = 0;
                                    if(avg > 0.0) {






                                    	for(int range_index = -580; range_index < 580; ++range_index) {

                                    		if( fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][sample_num + range_index] > max_value) {
                                    			max_value = fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][sample_num + range_index];
                                    			s_p = sample_num + range_index;
                                    		}

                                    	}

                                        //signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] = back_tracking_index;
										//vis_sample_pos_mapping[frequency_entry.first][channel_iterator] = back_tracking_index;

										signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] = s_p;
										vis_sample_pos_mapping[frequency_entry.first][channel_iterator] = s_p;


                                        //if(frequency_entry.first == 19000)
                                        //  std::cout << "19 KHZ @ sample: " << signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] << "\n";


                                        break;
                                    } else {
                                       // current_val = fft_transformer.signal_results_[channel_iterator][back_tracking_index];
                                    }

                                }

                                break;
                            }
                        }
                       */
 
                    } else {
                            signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] = sample_num;
                            vis_sample_pos_mapping[frequency_entry.first][channel_iterator] = sample_num;	
                    }     

                    ++sample_num;
                }



                /*
                for(apex_finder = sample_num + 700; apex_finder > 0; --apex_finder ) {
                	if( fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][apex_finder] < 0.7*sig )
                    	break;
                 }
						*/
                //unsigned sample_pos_1 = vis_sample_pos_mapping[frequency_entry.first][0];
                //unsigned sample_pos_2


            }

                unsigned sample_pos_array[4];

               	unsigned averaged_sample_pos = 0;
                for(unsigned int sp_it = 0; sp_it < 4; ++sp_it) {
                	sample_pos_array[sp_it] = vis_sample_pos_mapping[frequency_entry.first][sp_it];

                	if(sample_pos_array[sp_it] > averaged_sample_pos) {
                		averaged_sample_pos += sample_pos_array[sp_it];
                	}
                }

                averaged_sample_pos /= 4.0;

                unsigned closeness = std::numeric_limits<unsigned>::max();
                unsigned closest_sample_pos = 0;

                for(unsigned int sp_it = 0; sp_it < 4; ++sp_it) {

                	if( std::abs(sample_pos_array[sp_it] - averaged_sample_pos) < closeness ) {
                		closest_sample_pos = sample_pos_array[sp_it];
                		closeness =  std::abs(sample_pos_array[sp_it] - averaged_sample_pos);
                	}
                }


                for(unsigned channel_it = 0; channel_it < 4; ++channel_it) {
                   	unsigned apex_finder = closest_sample_pos;
                    for(apex_finder = closest_sample_pos; apex_finder > 0; --apex_finder ) {
                    	if( fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_it][apex_finder] < 1.0 )
                    		break;
                    	}
						
                        signal_detected_at_sample_per_frequency[frequency_entry.first][channel_it] = apex_finder;
                        vis_sample_pos_mapping[frequency_entry.first][channel_it] = apex_finder;		
            	}

        }

    }

    unsigned int const & (*min) (unsigned int const &, unsigned int const &) = std::min<unsigned>;
    unsigned int const & (*max) (unsigned int const &, unsigned int const &) = std::max<unsigned>;


    for(auto const& frequency_entry : frequency_toas_mapping) {

        unsigned sample_min = std::accumulate(signal_detected_at_sample_per_frequency[frequency_entry.first].begin(),
                                              signal_detected_at_sample_per_frequency[frequency_entry.first].end(),
                                              std::numeric_limits<unsigned>::max(), min);

        unsigned sample_max = std::accumulate(signal_detected_at_sample_per_frequency[frequency_entry.first].begin(),
                                                  signal_detected_at_sample_per_frequency[frequency_entry.first].end(),
                                                  0, max);

            //std::cout << "sample_min: " << sample_min << "\n";
        // if(frequency_entry.first > 50000) {
        //     std::cout << "Doing something\n";
        //     std::cout << "sample min: " << sample_min << "  sample_max: " << sample_max << "\n";  
        // }

        if(sample_max-sample_min > 0 && sample_max - sample_min < 500 && sample_max < 999999) {

            for(unsigned int signal_idx = 0; signal_idx < 4; ++signal_idx) {
                updated_times[signal_idx] = (signal_detected_at_sample_per_frequency[frequency_entry.first][signal_idx] - sample_min) / 44100.0;
                frequency_toas_mapping[frequency_entry.first][signal_idx] = updated_times[signal_idx];

                is_frequency_toa_mapping_valid[frequency_entry.first] = true;
                                       // std::cout << "Found something\n";
                                    //if(frequency_entry.first == 19000)
                                    //  std::cout << "valid: 19khz\n" ;
            }
        }
        else {
            is_frequency_toa_mapping_valid[frequency_entry.first] = false;
        }
    }
}

//frequencies we deregister are not taken into account in the analyzation step anymore
void Signal_Analyzer::
stop_listening_to(unsigned const frequency_to_stop_listening_to) {

    std::map<unsigned, std::array<double, 4> >::iterator frequency_iterator
        = frequency_toas_mapping.find(frequency_to_stop_listening_to);


    if( frequency_toas_mapping.end() != frequency_iterator ) {
        //delete the found entry:
        //we won't look for the corresponding toas anymore
        frequency_toas_mapping.erase(frequency_iterator);
    }
}

//only frequencies we register are taken into account for the analyzation
void Signal_Analyzer::
start_listening_to(unsigned const frequency_to_start_listening_to) {
	//just overwrite the slot, even if there's already an entry for
	//the frequency
	frequency_toas_mapping[frequency_to_start_listening_to]
		= std::array<double,4>();
}

std::array<double, 4> Signal_Analyzer::
get_toas_for(unsigned const frequency) {


    std::map<unsigned, std::array<double, 4> >::const_iterator frequency_iterator
        = frequency_toas_mapping.find(frequency);

        if( (frequency_toas_mapping.end() != frequency_iterator) && (is_frequency_toa_mapping_valid[frequency]) ) {

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

std::array<unsigned, 4> const Signal_Analyzer::
get_vis_sample_pos_for(unsigned const frequency) {

    std::map<unsigned, std::array<double, 4> >::const_iterator frequency_iterator
        = frequency_toas_mapping.find(frequency);

	//if( (frequency_toas_mapping.end() != frequency_iterator) && (is_frequency_toa_mapping_valid[frequency]) )  {
		return vis_sample_pos_mapping[frequency];
	/*} else {
		return {std::numeric_limits<unsigned>::max(), 
				std::numeric_limits<unsigned>::max(), 
				std::numeric_limits<unsigned>::max(), 
				std::numeric_limits<unsigned>::max()};
	}*/
}

std::array<std::vector<double>,4> const Signal_Analyzer::
get_signal_samples_for(unsigned const frequency) {

    return fft_transformer.signal_results_per_frequency_[frequency];
}