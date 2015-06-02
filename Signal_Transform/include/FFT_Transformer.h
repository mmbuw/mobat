
#ifndef FFT_TRANSFORMER_H
#define FFT_TRANSFORMER_H

#include "../include/ring_buffer.h"

#include <fftw3.h>
#include <iostream>
#include <cmath>


class FFT_Transformer {
	private:
		//std::map<unsigned int, fftw_plan> fftw_execution_plans_map_;


		float normalization_value;
		bool found_freq_;
		fftw_plan fftw_execution_plan_;
		unsigned int fft_window_size_;
		int* input_stream_;

		unsigned int start_sample_;
		unsigned int end_sample_;

		unsigned int audio_buffer_size_;
		unsigned int num_audio_buffers_;
		int** audio_buffers_;

		unsigned short fft_frame_size_;
		fftw_complex* fft_in_;
		fftw_complex* fft_result_;
		double* accumulated_result_;

		double* window_;

		double eighteen_khz_sum_;

		//double ring_buffer

		void create_hamming_window();
		void create_hann_window();
		void create_blackmann_harris_window();

	public:
		FFT_Transformer(unsigned short fft_frame_size);
		~FFT_Transformer();

		void initialize_execution_plan();
		void set_FFT_input( unsigned int offset);
		void perform_FFT();
		void print_FFT_result(unsigned int call_idx);

		void set_analyzation_range(unsigned int start_sample,
								   unsigned int end_sample);
	
		void set_FFT_buffers( unsigned int num_buffers,
							  unsigned int buffer_size,
							  int** buffers);
};

#endif
