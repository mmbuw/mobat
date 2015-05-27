
#ifndef FFT_TRANSFORMER_H
#define FFT_TRANSFORMER_H

#include <fftw3.h>
#include <iostream>
#include <cmath>

class FFT_Transformer {
	private:
		//std::map<unsigned int, fftw_plan> fftw_execution_plans_map_;

		bool found_freq_;
		fftw_plan fftw_execution_plan_;
		unsigned int fft_window_size_;
		int* input_stream_;

		unsigned short fft_frame_size_;
		fftw_complex* fft_in_;
		fftw_complex* fft_result_;
		double* accumulated_result_;

		double* hamming_window_;

		void create_hamming_window();

	public:
		FFT_Transformer(unsigned short fft_frame_size);
		~FFT_Transformer();

		void initialize_execution_plan();
		void set_FFT_input(unsigned int num_buffers, unsigned int buffer_size, int** buffers, unsigned int offset);
		void perform_FFT();
		void print_FFT_result(unsigned int call_idx);


	
};

#endif
