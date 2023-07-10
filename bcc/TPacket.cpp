#include "TPacket.h"

TPacket::TPacket() {
   //	m_packet = {NULL};
}

TPacket::~TPacket() {
	av_packet_unref(&m_packet);
}
