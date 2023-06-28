#include "codecCtx.h"

codecCtx::codecCtx() {
	ctx = NULL;
}

codecCtx::~codecCtx() {
	avcodec_close(ctx);
}