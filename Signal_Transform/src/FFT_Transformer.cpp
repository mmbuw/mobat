#include "../include/FFT_Transformer.h"

#define MATH_PI 3.14159265359 

//helper function
void FFT_Transformer::create_hamming_window() {

 for(int i = 0; i < fft_frame_size_; ++i) {
   hamming_window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_frame_size_ - 1) * 1.0))));
 }
}

FFT_Transformer::FFT_Transformer(unsigned short fft_frame_size) {
	found_freq_ = false;
	fft_in_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_frame_size);
	fft_result_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_frame_size);
	hamming_window_ = (double*) malloc(sizeof(double) * fft_frame_size);

	if(!fft_in_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	if(!fft_result_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	fft_frame_size_ = fft_frame_size;

	initialize_execution_plan();
	create_hamming_window();
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

	if(hamming_window_) {
		free(hamming_window_);
	}
};

void FFT_Transformer::initialize_execution_plan() {
	fftw_execution_plan_ = fftw_plan_dft_1d(fft_frame_size_, fft_in_, fft_result_,  FFTW_FORWARD, FFTW_MEASURE);
}

void FFT_Transformer::set_FFT_input(unsigned int num_buffers, unsigned int buffer_size, int** buffers, unsigned int offset) {
	
	//std::cout << "Buffersize: " << buffer_size << "\n";
	//for now, just use the first buffer, if available
	if(buffer_size >= fft_frame_size_) {
		if(num_buffers > 0) {
			for(unsigned int frame_idx = 0;
				frame_idx < fft_frame_size_;
				++frame_idx) {

				fft_in_[frame_idx][0] = hamming_window_[frame_idx] * (buffers[0][(offset) + frame_idx]) / 32767.0;
				fft_in_[frame_idx][1] = 0.0;
			}
		}
	} else {
		std::cout << "Not enough sample for FFT-Frame Size!\n";
		exit(-1);
	}


}

void FFT_Transformer::perform_FFT() {
	fftw_execute(fftw_execution_plan_);
}

void FFT_Transformer::print_FFT_result(unsigned int call_idx) {
	//unsigned counter = 0;
	for(unsigned int signal_it = 0;
		signal_it < (unsigned int) (fft_frame_size_ / 2 - 1);
		++signal_it) {

		float current_frequency = (signal_it * 44100) / fft_frame_size_;
		if(current_frequency > 18000.0 && current_frequency < 18002.0) {
			float current_amplitude 
				= std::sqrt( (fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
				  (fft_result_[signal_it][1]*fft_result_[signal_it][1]) );

			//if(++counter % 100 == 0)
			//if(call_idx % 100 == 0)
			if(current_amplitude > 8000) {

				if(!found_freq_) {
					std::cout << "it num: " << call_idx<<"  ";
					std::cout << current_frequency << "Hz : " << current_amplitude << "\n"; 
					found_freq_ = true;
				}
			}
		}
	}
	//std::cout << "\n";
}