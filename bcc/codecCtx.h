#pragma once

#include <stdlib.h>
extern "C" {
#include <libavcodec/avcodec.h>
}

class TcodecCtx
{
public:
	AVCodecContext* m_ctx;
	TcodecCtx();
	~TcodecCtx();
};

