#include "fft_transformer.h"

#include "configurator.hpp"

#include <limits>
#include <iostream>

#define MATH_PI 3.14159265359 

//helper function
void FftTransformer::createHammingWindow() {

  for(unsigned int i = 0; i < fft_window_size_; ++i) {
    window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_window_size_ - 1) * 1.0))));
    
    normalization_value += window_[i];
  }

  normalization_value /= (fft_window_size_/2);
}

void FftTransformer::createHannWindow() {
  for(unsigned int i = 0; i < fft_window_size_; ++i) {
    window_[i] = 0.5f * (1.0 - std::cos(2 * MATH_PI * i / (fft_window_size_-1.0)));
  }
}

void FftTransformer::createBlackmannHarrisWindow() {
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


void FftTransformer::resetThreadPerformedSignals() {

	for( int thread_idx = 0; thread_idx < 4; ++thread_idx) {
		ffts_performed_[thread_idx].store(false);
		allow_fft_[thread_idx].store(false);
	}

}

FftTransformer::FftTransformer(unsigned short fft_window_size) : start_sample_{ std::numeric_limits<unsigned int>::max() },
																  end_sample_{ std::numeric_limits<unsigned int>::max() }

																  	{


	resetThreadPerformedSignals();


	fft_threads_[0] = std::shared_ptr<std::thread>( new std::thread([&] { this->performFFTOnCertainChannel(0); } ) ); 
	fft_threads_[1] = std::shared_ptr<std::thread>( new std::thread([&] { this->performFFTOnCertainChannel(1); } ) ); 
	fft_threads_[2] = std::shared_ptr<std::thread>( new std::thread([&] { this->performFFTOnCertainChannel(2); } ) ); 
	fft_threads_[3] = std::shared_ptr<std::thread>( new std::thread([&] { this->performFFTOnCertainChannel(3); } ) ); 

    audio_buffers_ = (int32_t**) malloc( 4 * sizeof(int32_t*));

	fft_window_size_ = fft_window_size;


	found_freq_ = false;


	for(int i = 0; i < 4; ++i) {
		fft_in_[i] = (double*) fftw_malloc(sizeof(double) * fft_window_size_);
		fft_result_[i] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_window_size_);

	if(!fft_in_[i]) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	if(!fft_result_[i]) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}


	}
	window_ = (double*) malloc(sizeof(double) * fft_window_size_);



	initializeExecutionPlan();
	//createHammingWindow();
	//createBlackmannHarrisWindow();
	createHannWindow();
}

FftTransformer::~FftTransformer() {
	
	for(int i = 0; i < 4; ++i) {

		if(fftw_execution_plans_[i]) {
			fftw_destroy_plan(fftw_execution_plans_[i]);
		}
		
		if(fft_in_[i]) {
			fftw_free(fft_in_[i]);
		}

		if(fft_result_[i]) {
			fftw_free(fft_result_[i]);
		}

		//fftw_cleanup_threads();
	}

  if(window_) {
    free(window_);
  }

}


void FftTransformer::resetSampleCounters(unsigned channel_num) {

  for(unsigned iterated_frequency : listening_to_those_frequencies) {
    signal_results_per_frequency_[iterated_frequency][channel_num].clear();
    signal_results_per_frequency_[iterated_frequency][channel_num].reserve(5000);
  }
}

void FftTransformer::clearCachedFFTResults(unsigned channel_num) {

  for(unsigned iterated_frequency : listening_to_those_frequencies) {
    fft_cached_results_per_frequency_ [iterated_frequency][channel_num].clear();
  }
}

void FftTransformer::initializeExecutionPlan() {


	//fftw_plan_with_nthreads(1);
	for(int i = 0; i < 4; ++i) {
		fftw_execution_plans_[i] = fftw_plan_dft_r2c_1d(fft_window_size_, (fft_in_[i]), (fft_result_[i]), FFTW_EXHAUSTIVE);
	}
}

void FftTransformer::setAnalyzationRange(unsigned int start_sample, unsigned int end_sample, unsigned channel_num) {
	if(start_sample < end_sample) {
		start_sample_[channel_num] = start_sample;
		end_sample_[channel_num] = end_sample;
	}
}

void FftTransformer::setListenedFrequencies(std::vector<unsigned> const& listening_to_frequencies) {
  listening_to_those_frequencies = listening_to_frequencies;
}


void FftTransformer::performFFTOnCertainChannel(unsigned channel_iterator) {
	


	    std::cout << "Trying to perform fft with thread "<< channel_iterator << "\n";
		//thread is supposed to start here!

		while( true ) {

			if(allow_fft_[channel_iterator].load() == false ) {
				continue;
			}

			allow_fft_[channel_iterator].store(false);

	        unsigned signal_chunk = 1.0 * signal_half_chunk_;
	        resetSampleCounters(channel_iterator);
	        clearCachedFFTResults(channel_iterator);
	        for(unsigned int i = signal_chunk * (ints_per_channel_/num_chunks_); 
                           i < (signal_chunk+1)*(ints_per_channel_/num_chunks_) - 50; 
                           ++i) {
	            unsigned offset = 1 * i;
	            if(offset > (signal_chunk+1)*(ints_per_channel_/num_chunks_) - 50 )
	                break;
					
				setAnalyzationRange(0+offset, window_size_+50 + offset, channel_iterator);
	            

	            performFFT(channel_iterator);           
	        } 

	        //std::cout << "Signal half chunk: " << signal_half_chunk_ << "\n";
	        ffts_performed_[channel_iterator].store(true);
        }


}

void FftTransformer::performFFTOnChannels(buffer_collection const& signal_buffers, unsigned window_size, unsigned signal_half_chunk) {
    static float num_chunks = configurator().getFloat("num_splitted_fourier_chunks");

    for(int i = 0; i < 4; ++i) {
    	ffts_performed_[i].store(false);
    }

	ints_per_channel_ = signal_buffers.length;
	signal_half_chunk_ = signal_half_chunk;
	window_size_ = window_size;
	num_chunks_ = num_chunks;


    setFFTBuffers(4, ints_per_channel_, signal_buffers);

    for(int i = 0; i < 4; ++i) {
		  allow_fft_[i].store(true);
	  }


    while( (ffts_performed_[0].load() == false) ||
    	   (ffts_performed_[1].load() == false) ||
    	   (ffts_performed_[2].load() == false) ||
    	   (ffts_performed_[3].load() == false)  )
    {}    

  smoothResults();
}

void FftTransformer::setFFTBuffers(unsigned int num_buffers, unsigned int buffer_size, buffer_collection const& signal_buffers) {
	num_audio_buffers_ = num_buffers;
	audio_buffer_size_ = buffer_size;

	for(int buffer_idx = 0; buffer_idx < 4; ++buffer_idx) {
		audio_buffers_[buffer_idx] = signal_buffers[buffer_idx];	
	}

}


void FftTransformer::setFFTInput( unsigned int offset, unsigned int channel_num ) {
	
	if(audio_buffer_size_ >= fft_window_size_) {
		if(num_audio_buffers_ > 0) {
			for(unsigned int frame_idx = 0;
				frame_idx < fft_window_size_;
				++frame_idx) {

				//std::cout << "buffer content: " << audio_buffers_[0][(offset) + frame_idx] << "\n";
				fft_in_[channel_num][frame_idx] = window_[frame_idx] * (audio_buffers_[channel_num][(offset) + frame_idx]) / float(INT32_MAX);
			}
		}
	} else {
		std::cout << "Not enough sample for FFT-Frame Size!\n";
		exit(-1);
	}

}

void FftTransformer::smoothResults() {


  unsigned average_sample_num = 100;

  
  for(auto& frequency_slot : signal_results_per_frequency_) {
    for(auto& channel_results : frequency_slot.second) {


      std::size_t last_sample_idx = channel_results.size() - 1;

      std::vector<double> average_result_vector(channel_results.size(), 0.0);
      for(unsigned int sample_idx = 0; sample_idx <= last_sample_idx; ++sample_idx) {

        double average_frequency_sum = 0.0;
        for(unsigned int average_sample_idx = sample_idx; average_sample_idx < sample_idx + average_sample_num; ++average_sample_idx) {
          unsigned abs_idx_val = std::abs(average_sample_idx);
          unsigned int mirror_edge_index =  abs_idx_val > last_sample_idx ? abs_idx_val - (abs_idx_val % last_sample_idx ) : abs_idx_val;

          average_frequency_sum += channel_results[mirror_edge_index];
        }

        average_result_vector[sample_idx] = average_frequency_sum  / (float)(average_sample_num);
      }
      channel_results = average_result_vector;
    }
  }

};

unsigned int FftTransformer::performFFT(unsigned channel_num) {

	//dummy insertion
	//listening_to_those_frequencies.clear();
	//listening_to_those_frequencies.push_back(18000);
	//listening_to_those_frequencies.push_back(19000);
	//only perform an fft, if we listen to at least 1 frequency
	if(0 != listening_to_those_frequencies.size() ) {
		frequency_sums_[channel_num].clear();
		//eighteen_khz_sum_ = 0.0;


		if(start_sample_[channel_num] > end_sample_[channel_num] - (fft_window_size_ + 1) )
			return 0;

		for(unsigned int offset = start_sample_[channel_num]; offset <= end_sample_[channel_num] - (fft_window_size_ + 1) ; ++offset ) {


      //if(true) {

      //just check if the first frequency is cached, then we can safely assume that this is done
      //for all of the frequencies

      unsigned first_frequency_listened_to = *listening_to_those_frequencies.begin();
      if( fft_cached_results_per_frequency_[first_frequency_listened_to][channel_num].find(offset) 
        == fft_cached_results_per_frequency_[first_frequency_listened_to][channel_num].end() ) {
        //std::cout << "Window Size: " << fft_window_size_ << "\n";

				setFFTInput(offset, channel_num);
				fftw_execute(fftw_execution_plans_[channel_num]);

        //double temp_accum_18khz = 0.0;

        //initialize accumulation_vector
      std::map<unsigned, double> temp_accum_results;

      double normalization_range_value = 0.0;
      

        //unsigned int taken_normalization_samples = 0;
        //unsigned int taken_18_khz_samples = 0;

      for(unsigned int signal_it = 0;
        signal_it < (unsigned int) (fft_window_size_ / 2 - 1);
        ++signal_it) {
        float current_frequency = (signal_it * 44100.0) / fft_window_size_;



      if(current_frequency > 13000.0 && current_frequency < 20002.0 ) {

								double tmp_normalization_value = std::sqrt(fft_result_[channel_num][signal_it][0]*fft_result_[channel_num][signal_it][0]) + 
									  	(fft_result_[channel_num][signal_it][1]*fft_result_[channel_num][signal_it][1]) ;
								normalization_range_value +=  tmp_normalization_value;


        for(unsigned iterated_frequency : listening_to_those_frequencies) {
          if(current_frequency > ((double)iterated_frequency-100.0) && current_frequency < ((double)iterated_frequency+100.0 ) ) {

            temp_accum_results[iterated_frequency] += tmp_normalization_value;

          }
        }


      }

    }


    for(unsigned iterated_frequency : listening_to_those_frequencies) {

      double current_iteration_frequency_khz_sum = temp_accum_results[iterated_frequency] / normalization_range_value;
      fft_cached_results_per_frequency_[iterated_frequency][channel_num][offset] = current_iteration_frequency_khz_sum;

					frequency_sums_[channel_num][iterated_frequency] += current_iteration_frequency_khz_sum;
				}

			} else {
				for(unsigned iterated_frequency : listening_to_those_frequencies) {
					frequency_sums_[channel_num][iterated_frequency] += fft_cached_results_per_frequency_[iterated_frequency][channel_num][offset];
				}

  }




}

		//push back final results for this sample
		for(unsigned iterated_frequency : listening_to_those_frequencies) {
			if(!std::isnan(frequency_sums_[channel_num][iterated_frequency])) {

				signal_results_per_frequency_[iterated_frequency][channel_num].push_back(frequency_sums_[channel_num][iterated_frequency]);
				//std::cout << "Get results: " << frequency_sums_[iterated_frequency] << "\n";
			} else {

    std::cout << "NaN detected!!!!";

    std::cin.get();
        //return 2;
  }
}



}
return 0;

}
