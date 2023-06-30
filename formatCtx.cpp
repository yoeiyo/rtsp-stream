#include "formatCtx.h"

TformatCtx::TformatCtx() {
	m_ctx = nullptr;
}

TformatCtx::~TformatCtx() {
	if (m_ctx)
	avformat_close_input(&m_ctx);
}
