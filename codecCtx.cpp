#include "codecCtx.h"

TcodecCtx::TcodecCtx() {
	m_ctx = nullptr;
}

TcodecCtx::~TcodecCtx() {
	if (m_ctx)
	avcodec_close(m_ctx);
}