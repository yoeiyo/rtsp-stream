#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
}

class codecCtx
{
public:
	AVCodecContext* ctx;
	codecCtx();
	~codecCtx();
};

