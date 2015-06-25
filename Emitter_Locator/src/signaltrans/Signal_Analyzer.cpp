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
analyze(int** current_audio_buffer, unsigned int bytes_per_channel){


	std::vector<unsigned> fft_result_frequencies;
	for(auto const& frequency_to_transform_entry : frequency_toas_mapping) {
		fft_result_frequencies.push_back(frequency_to_transform_entry.first);

	}


	fft_transformer.set_listened_frequencies(fft_result_frequencies);


	signal_detected_at_sample_per_frequency.clear();




    fft_transformer.perform_FFT_on_channels((int**)&current_audio_buffer[0], bytes_per_channel, fft_window_size);


        double updated_times[4];


        unsigned starting_sample_threshold =  50;



        double signal_average[4] ;



			for(auto const& frequency_entry : frequency_toas_mapping) {

				//std::cout << "Handling " << frequency_entry.first << "\n";

        	    double const & (*d_max) (double const &, double const &) = std::max<double>;

    				signal_detected_at_sample_per_frequency[frequency_entry.first] = {100000, 100000, 100000, 100000};


    	    	for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {


		            double sum = std::accumulate(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
		                                           fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), 0.0, d_max);
		            signal_average[channel_iterator] = sum ;/// fft_transformer.signal_results_[channel_iterator].size();

		            unsigned num_pause_samples = 0;
		            unsigned num_signal_samples = 0;

		            unsigned sample_num = 0;

		            double peak = signal_average[channel_iterator];


		            double avg = std::accumulate(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].begin(), 
		                                           fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].end(), 0) /  fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator].size();

		            bool allow_signal_detection = false;

		            for(auto const& sig : fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator]) {


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


		                            for(int back_tracking_index = sample_num-1; back_tracking_index >= 0; --back_tracking_index) {

		                                if(fft_transformer.signal_results_per_frequency_[frequency_entry.first][channel_iterator][back_tracking_index] < avg *0.50) {
		                                    signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] = back_tracking_index;

											//if(frequency_entry.first == 19000)
											//	std::cout << "19 KHZ @ sample: " << signal_detected_at_sample_per_frequency[frequency_entry.first][channel_iterator] << "\n";

		                                    /*'if(channel_iterator == 0) {
		                                    	std::cout << "Detected signal\n";
		                                    	std::cin.get();
		                                    }*/
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



		        if(sample_max > 9999)
		        	return;



		        //std::cout << "sample_min: " << sample_min << "\n";

		        if(sample_max-sample_min > 0 && sample_max - sample_min < 500 && sample_max < 999999) {

			        for(unsigned int signal_idx = 0; signal_idx < 4; ++signal_idx) {
			            updated_times[signal_idx] = (signal_detected_at_sample_per_frequency[frequency_entry.first][signal_idx] - sample_min) / 44100.0;
			            frequency_toas_mapping[frequency_entry.first][signal_idx] = updated_times[signal_idx];

			            is_frequency_toa_mapping_valid[frequency_entry.first] = true;
			                                   // std::cout << "Found something\n";
			                                   // std::cin.get();

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
		//		an empty array
		else {

			return {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()};
		}

}

std::array<std::vector<double>,4> const Signal_Analyzer::
get_signal_samples_for(unsigned const frequency) {

	return fft_transformer.signal_results_per_frequency_[frequency];
}