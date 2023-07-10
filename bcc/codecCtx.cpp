#include "codecCtx.h"

TcodecCtx::TcodecCtx() {
	m_ctx = NULL;
}

TcodecCtx::~TcodecCtx() {
	if (m_ctx)
	avcodec_close(m_ctx);
}