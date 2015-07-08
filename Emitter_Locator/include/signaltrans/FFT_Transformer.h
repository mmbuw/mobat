
#ifndef FFT_TRANSFORMER_H
#define FFT_TRANSFORMER_H

#include "ring_buffer.h"

#include <fftw3.h>
#include <iostream>
#include <cmath>

#include <map>

#include <vector>

class FFT_Transformer {
	private:

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
		double* fft_in_;
		fftw_complex* fft_result_;
		double* accumulated_result_;

		double* window_;

		//double eighteen_khz_sum_;
		std::map<unsigned, double> frequency_sums_;


		unsigned stabilization_counter_;

		unsigned int fft_frame_count_;

		std::vector<unsigned> listening_to_those_frequencies;


		void create_hamming_window();
		void create_hann_window();
		void create_blackmann_harris_window();

	public:
		FFT_Transformer(unsigned short fft_frame_size);
		~FFT_Transformer();

		void reset_sample_counters(unsigned );
		void clear_cached_fft_results(unsigned);

		void initialize_execution_plan();

		void set_listened_frequencies(std::vector<unsigned> const& listening_to_frequencies);

		void perform_FFT_on_channels(int** signal_buffers, unsigned bytes_per_channel, unsigned window_size, unsigned signal_chunk);

		void set_FFT_input( unsigned int offset);
		unsigned perform_FFT(unsigned );

		void set_analyzation_range(unsigned int start_sample,
								   unsigned int end_sample);
	
		void set_FFT_buffers( unsigned int num_buffers,
							  unsigned int buffer_size,
							  int** buffers);

		std::map<unsigned, std::array<std::vector<double>,4> > signal_results_per_frequency_;

		//frequency, 4 channels, for each offset a value of fft sums
		std::map<unsigned, std::array<std::map<unsigned, double>,4>> fft_cached_results_per_frequency_;
};

#endif
