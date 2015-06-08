#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <iostream>

template <class DATA_TYPE>
class Ring_Buffer {

	public:
		Ring_Buffer(unsigned num_elements) : num_filled_elements_(0) {
			ring_data_ = (DATA_TYPE*) malloc(sizeof(DATA_TYPE) * num_elements);

			if(!ring_data_) {
				std::cerr << "Unable to allocate data for ring buffer\n";

				is_allocated_ = false;
			} else {

				capacity_ = num_elements;
				is_allocated_  = true;
			}
		}

		~Ring_Buffer() {

			if(!ring_data_) {
				std::cerr << "Can not destroy empty ring buffer\n";
				return;
			}

			free(ring_data_); 
		}

		//reads num_samples from the current tail of the ringbuffer into the provided Buffer
		void Read_Samples(unsigned const num_samples, 
						  DATA_TYPE* buffer_to_write) const;

		void Write_Samples(unsigned const num_samples,
						  DATA_TYPE* buffer_to_read);

		//pushes one sample at the current head of the ringbuffer
		void Push_Sample(DATA_TYPE* buffer_to_read);

		void Reset_Buffer(unsigned new_buffer_size);

		unsigned Get_Capacity() const;

	private:
		unsigned capacity_;
		unsigned num_filled_elements_;

		DATA_TYPE* ring_data_;

		bool is_allocated_;

		unsigned head_;
		unsigned tail_;

};

template class Ring_Buffer<double>;

#endif