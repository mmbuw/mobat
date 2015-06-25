#include "FFT_Transformer.h"

#include <limits>
#include <iostream>


#define MATH_PI 3.14159265359 

//helper function
void FFT_Transformer::create_hamming_window() {

 for(unsigned int i = 0; i < fft_window_size_; ++i) {
   window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_window_size_ - 1) * 1.0))));
 
 	normalization_value += window_[i];
 }

 normalization_value /= (fft_window_size_/2);
}

void FFT_Transformer::create_hann_window() {
 for(unsigned int i = 0; i < fft_window_size_; ++i) {
   window_[i] = 0.5f * (1.0 - std::cos(2 * MATH_PI * i / (fft_window_size_-1.0)));
 
 	//normalization_value += window_[i];
 }
}

void FFT_Transformer::create_blackmann_harris_window() {
	float a0 = 0.35875f;
	float a1 = 0.48829f;
	float a2 = 0.14128f;
	float a3 = 0.01168f;

	normalization_value = 1.0;
 for(unsigned int i = 0; i < fft_window_size_; ++i) {
   //window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_window_size_ - 1) * 1.0))));
 	window_[i] =   a0 
 				 - a1 * std::cos( 2 * MATH_PI * i / (fft_window_size_ - 1))
 				 + a2 * std::cos( 4 * MATH_PI * i / (fft_window_size_ - 1))
 				 - a3 * std::cos( 6 * MATH_PI * i / (fft_window_size_ - 1));


 		normalization_value += window_[i]*window_[i];
 }	
 normalization_value = std::sqrt(normalization_value);
}

FFT_Transformer::FFT_Transformer(unsigned short fft_window_size) : start_sample_( std::numeric_limits<unsigned int>::max() ),
																  end_sample_( std::numeric_limits<unsigned int>::max() ) {
	/*int fftw_thread_success = fftw_init_threads();

	if(fftw_thread_success == 0) {
		std::cout << "Could not initialize threads for FFTW\n";
	} else {
		std::cout << "Successfully initialized threads for FFTW\n";
	}*/

	fft_window_size_ = fft_window_size;


	found_freq_ = false;
	fft_in_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_window_size_);
	fft_result_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_window_size_);
	window_ = (double*) malloc(sizeof(double) * fft_window_size_);

	if(!fft_in_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	if(!fft_result_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	initialize_execution_plan();
	//create_hamming_window();
	//create_blackmann_harris_window();
	create_hann_window();
}

FFT_Transformer::~FFT_Transformer() {
	if(fftw_execution_plan_) {
		fftw_destroy_plan(fftw_execution_plan_);
	}

	if(fft_in_) {
		fftw_free(fft_in_);
	}

	if(fft_result_) {
		fftw_free(fft_result_);
	}

	if(window_) {
		free(window_);
	}

	//fftw_cleanup_threads();
};

void FFT_Transformer::reset_sample_counters(unsigned channel_num) {

	for(unsigned iterated_frequency : listening_to_those_frequencies) {
		signal_results_per_frequency_[iterated_frequency][channel_num].clear();
		signal_results_per_frequency_[iterated_frequency][channel_num].reserve(5000);
	}
}

void FFT_Transformer::clear_cached_fft_results(unsigned channel_num) {

	for(unsigned iterated_frequency : listening_to_those_frequencies) {
		fft_cached_results_per_frequency_	[iterated_frequency][channel_num].clear();
	}
}

void FFT_Transformer::initialize_execution_plan() {

	//fftw_plan_with_nthreads(1);
	fftw_execution_plan_ = fftw_plan_dft_1d(fft_window_size_, fft_in_, fft_result_,  FFTW_FORWARD, FFTW_MEASURE);
}

void FFT_Transformer::set_analyzation_range(unsigned int start_sample, unsigned int end_sample) {
	if(start_sample < end_sample) {
		start_sample_ = start_sample;
		end_sample_ = end_sample;
	}
}

void FFT_Transformer::set_listened_frequencies(std::vector<unsigned> const& listening_to_frequencies) {
	listening_to_those_frequencies = listening_to_frequencies;
}

void FFT_Transformer::perform_FFT_on_channels(int** signal_buffers, unsigned bytes_per_channel, unsigned window_size) {

        for (unsigned int channel_iterator = 0; channel_iterator < 4	; ++channel_iterator) {
            set_FFT_buffers(4, 
                    bytes_per_channel,
                (int**)&signal_buffers[channel_iterator]);   


            reset_sample_counters(channel_iterator);
            clear_cached_fft_results(channel_iterator);
            for(unsigned int i = 0; i < 5000; ++i) {
                unsigned offset = 1 * i;
                if(offset > (5000) )
                    break;
					
				set_analyzation_range(0+offset, window_size+50 + offset);
                

                perform_FFT(channel_iterator);           
            } 
        }
}

void FFT_Transformer::set_FFT_buffers(unsigned int num_buffers, unsigned int buffer_size, int** buffers) {
	num_audio_buffers_ = num_buffers;
	audio_buffer_size_ = buffer_size;
	audio_buffers_ = buffers;
}
void FFT_Transformer::set_FFT_input( unsigned int offset ) {
	
	if(audio_buffer_size_ >= fft_window_size_) {
		if(num_audio_buffers_ > 0) {
			for(unsigned int frame_idx = 0;
				frame_idx < fft_window_size_;
				++frame_idx) {

				//std::cout << "buffer content: " << audio_buffers_[0][(offset) + frame_idx] << "\n";
				fft_in_[frame_idx][0] = window_[frame_idx] * (audio_buffers_[0][    (offset) + frame_idx]) / 2147483647.0;
				fft_in_[frame_idx][1] = 0.0;
			}
		}
	} else {
		std::cout << "Not enough sample for FFT-Frame Size!\n";
		exit(-1);
	}

 
}

unsigned int FFT_Transformer::perform_FFT(unsigned channel_num) {

	//only perform an fft, if we listen to at least 1 frequency
	if(0 != listening_to_those_frequencies.size() ) {
		frequency_sums_.clear();
		//eighteen_khz_sum_ = 0.0;


		if(start_sample_ > end_sample_ - (fft_window_size_ + 1) )
			return 0;

		for(unsigned int offset = start_sample_; offset <= end_sample_ - (fft_window_size_ + 1) ; ++offset ) {
			//std::cout << "Trying offset " << offset <<"\n";

			//if(true) {

			//just check if the first frequency is cached, then we can safely assume that this is done
			//for all of the frequencies

			unsigned first_frequency_listened_to = *listening_to_those_frequencies.begin();
			if( fft_cached_results_per_frequency_[first_frequency_listened_to][channel_num].find(offset) 
				== fft_cached_results_per_frequency_[first_frequency_listened_to][channel_num].end() ) {
				//std::cout << "Window Size: " << fft_window_size_ << "\n";

				set_FFT_input(offset);
				fftw_execute(fftw_execution_plan_);

				//double temp_accum_18khz = 0.0;

				//initialize accumulation_vector
				std::vector<double> temp_accum_results(listening_to_those_frequencies.size(), 0.0);

				//double normalization_range_value = 0.0;
				

				//unsigned int taken_normalization_samples = 0;
				//unsigned int taken_18_khz_samples = 0;

				for(unsigned int signal_it = 0;
					signal_it < (unsigned int) (fft_window_size_ / 2 - 1);
					++signal_it) {
						float current_frequency = (signal_it * 44100) / fft_window_size_;
	/*
							if(current_frequency > 13000.0 && current_frequency < 20000.0 ) {
								normalization_range_value +=  std::sqrt(fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
									  	(fft_result_[signal_it][1]*fft_result_[signal_it][1]) ;

									  	++taken_normalization_samples;
							}
	*/

							for(unsigned iterated_frequency : listening_to_those_frequencies) {
								if(current_frequency > ((double)iterated_frequency-100.0) && current_frequency < ((double)iterated_frequency+100.0 ) ) {
								    /*temp_accum_18khz += std::sqrt(fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
										  	(fft_result_[signal_it][1]*fft_result_[signal_it][1]) ;*/

									temp_accum_results[iterated_frequency] += std::sqrt(fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
																		  	(fft_result_[signal_it][1]*fft_result_[signal_it][1]) ;

										  	//++taken_18_khz_samples;
								//std::cout << "Current Frequency: " << current_frequency << ": " << temp_accum_18khz<< "\n";
								}
							}




				}

				//double current_iteration_khz_sum = temp_accum_18khz;// / normalization_range_value;



				for(unsigned iterated_frequency : listening_to_those_frequencies) {

					double current_iteration_frequency_khz_sum = temp_accum_results[iterated_frequency];
					fft_cached_results_per_frequency_[iterated_frequency][channel_num][offset] = current_iteration_frequency_khz_sum;

					frequency_sums_[iterated_frequency] = current_iteration_frequency_khz_sum;
				}

				//frequency_sums_[]
				//eighteen_khz_sum_ += current_iteration_khz_sum;
				//std::cout << "Uncached result\n";
			} else {
				for(unsigned iterated_frequency : listening_to_those_frequencies) {
					frequency_sums_[iterated_frequency] += fft_cached_results_per_frequency_[iterated_frequency][channel_num][offset];
				}
				//std::cout << "Cached result\n";		
			}




		}

		//push back final results for this sample
		for(unsigned iterated_frequency : listening_to_those_frequencies) {
			if(!std::isnan(frequency_sums_[iterated_frequency])) {

				signal_results_per_frequency_[iterated_frequency][channel_num].push_back(frequency_sums_[iterated_frequency]);
			
			} else {

				std::cout << "NaN detected!!!!";

				std::cin.get();
				//return 2;
			}
		}



	}
	return 0;
}
