#pragma once

#include <stdlib.h>
extern "C" {
#include <libavformat/avformat.h>
}


class TformatCtx
{
public:
	AVFormatContext* m_ctx;

	TformatCtx();
	~TformatCtx();

	/*
	int openCtx_input(char* inp_url);
	int stream_info();
	void dump_format();
	*/
};

