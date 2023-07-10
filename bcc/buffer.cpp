#include "buffer.h"

Tbuffer::Tbuffer() {
	m_buff = NULL;
}

Tbuffer::Tbuffer(int numBytes) {
	m_buff = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
}

Tbuffer::~Tbuffer() {
	if(m_buff)
	av_free(m_buff);
}