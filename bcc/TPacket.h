#pragma once

#include <stdlib.h>
extern "C"
{
#include <libavformat/avformat.h>
}

class TPacket
{
public:

	AVPacket m_packet;
	TPacket();
	~TPacket();
};

