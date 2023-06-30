#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

class Tbuffer
{
public:
	uint8_t* m_buff;
	Tbuffer();
	Tbuffer(int numBytes);
	~Tbuffer();

};

