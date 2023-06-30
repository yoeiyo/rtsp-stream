#pragma once
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

