#include "frame.h"

Tframe::Tframe() {
	m_fr = nullptr;
	//buff = NULL;
}

Tframe::~Tframe() {
	if (m_fr)
	av_free(m_fr);
//	av_free(buff);
}