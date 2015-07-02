#include "buffer_collection.hpp"
#include <stdexcept>

buffer_collection::buffer_collection(std::size_t bufferbytes, std::size_t buffernum) :
 count{buffernum},
 bytes{bufferbytes},
 length{bufferbytes / 4}
{
	buffers = (int**)malloc(buffernum * sizeof(int*));

	if(buffers) {
		for(std::size_t i = 0; i < count; ++i) {
			buffers[i] = (int*)malloc(bufferbytes);
			if(!buffers[i]) {
				throw std::logic_error("memory allocation failed");
			}
		}
	}
	else throw std::logic_error("memory allocation failed");
}

buffer_collection::~buffer_collection() {
	for(std::size_t i = 0; i < count; ++i) {
		if(buffers[i]) {
			free(buffers[i]);
			buffers[i] = nullptr;
		}
	}
	if(buffers) {
		free(buffers);
	}
}

//copy data from interleaved buffer, assumes that the sourcesize equals length * count 
void buffer_collection::from_interleaved(unsigned char* source) {
	// fill each channel
	for(std::size_t current_buffer = 0; current_buffer < count; ++current_buffer) {
		// and each int
	   for(std::size_t buffer_offset_pos = 0; buffer_offset_pos < length; ++buffer_offset_pos) {

			buffers[current_buffer][buffer_offset_pos] 
				= 0x0 
				| source[buffer_offset_pos * (count*4)      + current_buffer * 4 ]  <<  0
				| source[buffer_offset_pos * (count*4) + 1  + current_buffer * 4 ]  <<  8
				| source[buffer_offset_pos * (count*4) + 2  + current_buffer * 4 ]  <<  16
				| source[buffer_offset_pos * (count*4) + 3  + current_buffer * 4 ]  <<  24;
		}
	}
}

int* const& buffer_collection::operator[](std::size_t i) {
	return buffers[i];
}

int* const& buffer_collection::operator[](std::size_t i) const {
	return buffers[i];
}
