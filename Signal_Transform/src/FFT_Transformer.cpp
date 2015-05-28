#include "../include/FFT_Transformer.h"

#define MATH_PI 3.14159265359 

//helper function
void FFT_Transformer::create_hamming_window() {

 for(int i = 0; i < fft_frame_size_; ++i) {
   window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_frame_size_ - 1) * 1.0))));
 
 	normalization_value += window_[i];
 }

 normalization_value /= (fft_frame_size_/2);
}

void FFT_Transformer::create_hann_window() {
 for(int i = 0; i < fft_frame_size_; ++i) {
   window_[i] = 0.5f * (1.0 - std::cos(2 * MATH_PI * i / (fft_frame_size_-1.0)));
 
 	//normalization_value += window_[i];
 }
}

void FFT_Transformer::create_blackmann_harris_window() {
	float a0 = 0.35875f;
	float a1 = 0.48829f;
	float a2 = 0.14128f;
	float a3 = 0.01168f;

	normalization_value = 1.0;
 for(int i = 0; i < fft_frame_size_; ++i) {
   //window_[i] = 0.54f - (0.46f * std::cos( 2 * MATH_PI * (i / ((fft_frame_size_ - 1) * 1.0))));
 	window_[i] =   a0 
 				 - a1 * std::cos( 2 * MATH_PI * i / (fft_frame_size_ - 1))
 				 + a2 * std::cos( 4 * MATH_PI * i / (fft_frame_size_ - 1))
 				 - a3 * std::cos( 6 * MATH_PI * i / (fft_frame_size_ - 1));


 		normalization_value += window_[i]*window_[i];
 }	
 normalization_value = std::sqrt(normalization_value);
}

FFT_Transformer::FFT_Transformer(unsigned short fft_frame_size) {
	int fftw_thread_success = fftw_init_threads();

	if(fftw_thread_success == 0) {
		std::cout << "Could not initialize threads for FFTW\n";
	} else {
		std::cout << "Successfully initialized threads for FFTW\n";
	}
	found_freq_ = false;
	fft_in_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_frame_size);
	fft_result_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_frame_size);
	window_ = (double*) malloc(sizeof(double) * fft_frame_size);

	if(!fft_in_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	if(!fft_result_) {
		std::cout << "Failed to allocate memory for FFT input buffer\n";
	}

	fft_frame_size_ = fft_frame_size;

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

	fftw_cleanup_threads();
};

void FFT_Transformer::initialize_execution_plan() {

	fftw_plan_with_nthreads(1);
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

				fft_in_[frame_idx][0] = window_[frame_idx] * (buffers[0][(offset) + frame_idx]) / 32767.0;
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
	//double normalization_divident = 2048;

	double normalization_divident = 0.0;
	for(unsigned int signal_it = 0;
		signal_it < (unsigned int) (fft_frame_size_ / 2 - 1);
		++signal_it) {


		if( (signal_it * 44100) / fft_frame_size_ > 15000.0)
		normalization_divident += std::sqrt( (fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
				  (fft_result_[signal_it][1]*fft_result_[signal_it][1]) );

	} 
		double freq_sum = 0.0;
		//double eighteen_khz_area = 0.0;
	for(unsigned int signal_it = 0;
		signal_it < (unsigned int) (fft_frame_size_ / 2 );
		++signal_it) {


		float current_frequency = (signal_it * 44100) / fft_frame_size_;
		//if(current_frequency > 18000.0 && current_frequency < 18002.0) {
			float current_amplitude 
				= std::sqrt( (fft_result_[signal_it][0]*fft_result_[signal_it][0]) + 
				  (fft_result_[signal_it][1]*fft_result_[signal_it][1]) );
	//			freq_sum += current_amplitude;
			//if(++counter % 100 == 0)
			//if(call_idx % 100 == 0)
			//if(current_amplitude > 8000) {
if(signal_it == 0 || signal_it == fft_frame_size_/2)
	current_amplitude /= 2.0;

	current_amplitude = ((current_amplitude/(fft_frame_size_/2)))/fft_frame_size_;
				//if(!found_freq_) {
		//			std::cout << "it num: " << call_idx<<"  ";
									if(current_frequency > 18000.0 && current_frequency < 18200.0 )
										//if(call_idx % 100 == 0)
					//			eighteen_khz_area += /*10*log10*/current_amplitude; /// normalization_divident;
								//if(20*log10(current_amplitude) > -42.0)
								std::cout << "it: " << call_idx << "  " << current_frequency << "Hz : " << 20*log10(current_amplitude) << "\n"; 

				//if(current_frequency > 15000.0)
					freq_sum += current_amplitude ;
					found_freq_ = true;
				//}
			//}
		//}	

	}
	//if(call_idx % 1000 == 0)
	//	std::cout << "sum: " << freq_sum << "\n";
	//std::cout << eighteen_khz_area << "\n";
//std::cout << "norm val:" << normalization_value << "\n";
	//					std::cout << "Frequency_Sum = " << freq_sum << "\n";
	//std::cout << "\n";
}