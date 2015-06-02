#include "../include/ring_buffer.h"

#include <cstdlib>
#include <cstring>
#include <iostream>


template <class DATA_TYPE>
Ring_Buffer<DATA_TYPE>::Ring_Buffer(unsigned num_elements) : num_filled_elements_(0) {
	ring_data_ = (DATA_TYPE*) malloc(sizeof(DATA_TYPE) * num_elements);

	if(!ring_data_) {
		std::cout << "Unable to allocate data for ring buffer\n";

		is_allocated_ = false;
	} else {

		capacity_ = num_elements;
		is_allocated_  = true;
	}
}


template <class DATA_TYPE>
Ring_Buffer<DATA_TYPE>::~Ring_Buffer() {

	if(!ring_data_) {
		std::cout << "Can not destroy empty ring buffer\n";
		return;
	}

	free(ring_data_); 
}



template <class DATA_TYPE>
void Ring_Buffer<DATA_TYPE>::Read_Samples(unsigned const num_samples, 
							   DATA_TYPE* buffer_to_write) const {
	if(capacity_ < num_samples) {
		std::cout << "Can not read " << num_samples << " samples,"
				  <<" because the ring buffer capacity is " << capacity_ 
				  <<"\n";

		return;
	}

	unsigned reading_pos = tail_;
	for( unsigned writing_pos = 0; writing_pos < num_samples; ++writing_pos ) {

		buffer_to_write[writing_pos] = ring_data_[reading_pos];

		reading_pos = (reading_pos + 1) % capacity_;
	}
}

template <class DATA_TYPE>
void Ring_Buffer<DATA_TYPE>::Write_Samples(unsigned const num_samples,
								  DATA_TYPE* buffer_to_read) {
	if(capacity_ < num_samples) {
		std::cout << "Can not write " << num_samples << " samples,"
				  <<" because the ring buffer capacity is " << capacity_
				  <<"\n";		
		return;
	}

	bool first_write = false;
	if(num_filled_elements_ != 0) {
		tail_ = 0;

		first_write = true;
	}

	memcpy(&ring_data_[tail_], buffer_to_read, sizeof(DATA_TYPE)*num_samples);

	if(first_write) {
		tail_ = num_filled_elements_;
		head_ = 1;
	} else {
		tail_ = (tail_ + num_filled_elements_) % capacity_;
	}

	head_ = (head_ + num_filled_elements_) % capacity_;

}

template <class DATA_TYPE>
void Ring_Buffer<DATA_TYPE>::Push_Sample(DATA_TYPE* buffer_to_read) {
	Write_Samples(1, buffer_to_read);
}

template <class DATA_TYPE>
void Ring_Buffer<DATA_TYPE>::Reset_Buffer(unsigned new_buffer_size) {
	if(!ring_data_) {
		free(ring_data_);
		ring_data_ = (DATA_TYPE*) malloc(sizeof(DATA_TYPE) * new_buffer_size);

		if(!ring_data_) {
			std::cout << "Could not reallocate ring data\n";
		}
	} else {
		std::cout << "Could not deallocate empty ring data\n";
	}

	capacity_ = new_buffer_size;
	is_allocated_ = true;
	num_filled_elements_ = 0;

};
