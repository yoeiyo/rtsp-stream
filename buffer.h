#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

class buffer
{
public:
	uint8_t* buff;
	buffer();
	buffer(int numBytes);
	~buffer();

};

