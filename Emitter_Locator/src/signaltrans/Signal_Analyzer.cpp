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


	//frequency_toas_mapping.clear();

    signal_detected_at_sample = {100000, 100000, 100000, 100000};

    fft_transformer.perform_FFT_on_channels((int**)&current_audio_buffer[0], bytes_per_channel, fft_window_size);


        double updated_times[4];


        unsigned starting_sample_threshold =  50;



        unsigned signal_average[4] ;

        for(unsigned int channel_iterator = 0; channel_iterator < 4; ++channel_iterator) {


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
                                              std::numeric_limits<unsigned>::max(), min);

        unsigned sample_max = std::accumulate(signal_detected_at_sample.begin(),
                                              signal_detected_at_sample.end(),
                                              0, max);

        if(sample_max > 9999)
        	return;



        std::cout << "\n\n\ndetected samples: " <<
        			signal_detected_at_sample[0] << "\n" << 
         			signal_detected_at_sample[1] << "\n" << 
         			signal_detected_at_sample[2] << "\n" << 
         			signal_detected_at_sample[3] << "\n\n";

        std::cout << "sample_min: " << sample_min << "\n";

        if(sample_max - sample_min < 500 && sample_max < 999999) {

	        for(unsigned int signal_idx = 0; signal_idx < 4; ++signal_idx) {
	            updated_times[signal_idx] = (signal_detected_at_sample[signal_idx] - sample_min) / 44100.0;
	            frequency_toas_mapping[18000][signal_idx] = updated_times[signal_idx];
	                                   // std::cout << "Found something\n";
	                                   // std::cin.get();

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
get_toas_for(unsigned const frequency) const{

	std::map<unsigned, std::array<double, 4> >::const_iterator frequency_iterator
		= frequency_toas_mapping.find(frequency);

		if(frequency_toas_mapping.end() != frequency_iterator) {

			std::cout << "toas in getter: " << 
				frequency_iterator->second[0] << "\n" <<
				frequency_iterator->second[1] << "\n" <<
				frequency_iterator->second[2] << "\n" <<
				frequency_iterator->second[3] << "\n\n";
			return frequency_iterator->second;
		} 

		//TODO: add "Frequency not registered exception" instead of returning
		//		an empty array
		else {

			return std::array<double, 4>();
		}

}

std::array<std::vector<double>,4> const Signal_Analyzer::
get_signal_samples_for(unsigned const frequency) const{

	return fft_transformer.signal_results_;
}