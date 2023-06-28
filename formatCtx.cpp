#include "formatCtx.h"

formatCtx::formatCtx() {
	ctx = NULL;
}

formatCtx::~formatCtx() {
	avformat_close_input(&ctx);
}
