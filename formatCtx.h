#pragma once
extern "C" {
#include <libavformat/avformat.h>
}


class formatCtx
{
public:
	AVFormatContext* ctx; 

	formatCtx();
	~formatCtx();

	/*
	int openCtx_input(char* inp_url);
	int stream_info();
	void dump_format();
	*/
};

