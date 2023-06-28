#include "buffer.h"

buffer::buffer() {
	buff = NULL;
}

buffer::buffer(int numBytes) {
	buff = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
}

buffer::~buffer() {
	av_free(buff);
}