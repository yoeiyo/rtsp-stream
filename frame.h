#pragma once
extern "C" {
#include <libavformat/avformat.h>
}

class frame
{
public: 
	AVFrame* fr;
	//uint8_t* buff;
	frame();
	~frame();
};
