#pragma once
extern "C" {
#include <libavformat/avformat.h>
}

class Tframe
{
public: 
	AVFrame* m_fr;
	//uint8_t* buff;
	Tframe();
	~Tframe();
};
