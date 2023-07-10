#include "formatCtx.h"


TformatCtx::TformatCtx() {
	m_ctx = NULL;
}

TformatCtx::~TformatCtx() {
	if (m_ctx)
	avformat_close_input(&m_ctx);
}
