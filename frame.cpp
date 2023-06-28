#include "frame.h"

frame::frame() {
	fr = NULL;
	//buff = NULL;
}

frame::~frame() {
	av_free(fr);
//	av_free(buff);
}