//---------------------------------------------------------------------------

#include <vcl.h>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
/*
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
*/

extern "C"
{
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/avconfig.h>
#include <libavutil/avutil.h>
}

//#include <stdlib.h>

#pragma hdrstop

#include "formatCtx.h"
#include "buffer.h"
#include "codecCtx.h"
#include "frame.h"
#include "TPacket.h"

#include "Unit4.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;



int connect(TformatCtx pFormatCtx){

	char* sourceUrl = new char[Form4->conSource->Text.Length() + 1];
	wcstombs(sourceUrl,Form4->conSource->Text.c_str(),Form4->conSource->Text.Length() + 1);
	int nRes = avformat_open_input(&pFormatCtx.m_ctx, sourceUrl, NULL, NULL);
	delete sourceUrl;
	if (nRes < 0) {// sometimes stuck here; not sure what the problem is
		// most likely it can't properly connect
		// stuck at line 755 (ret = ff_rtsp_connect(s);) in rtspdec.c

		Form4->Label1->Caption = "couldn't connect\n";
	}
	return nRes;
}

int findStream(TformatCtx pFormatCtx){

	Form4->Label1->Caption = "connection successful\ntrying to find stream info...\n";
	//Form4->Label1->Caption = "trying to find stream info...\n";

	 // Retrieve stream information
	int nRes = avformat_find_stream_info(pFormatCtx.m_ctx, NULL);
	if (nRes < 0) {
		Form4->Label1->Caption = "Couldn't find stream information\n";
		//return -1; // Couldn't find stream information
	}
	Form4->Label1->Caption = "found stream information\n";
	return nRes;
}

int showFrame(AVFrame* pFrame, int width, int height, int iFrame){
int p, r, g, b;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
		p = x*3+y*pFrame->linesize[0];
		r=pFrame->data[0][p];
		g=pFrame->data[0][p+1];
		b=pFrame->data[0][p+2];
		Form4->Canvas->Pixels[20 + x][20 + y] = (TColor) RGB (r, g, b);
		}

	return 0;
}

char* main(){

	// Open video file
	//AVFormatContext* pFormatCtx = NULL;

	Form4->Label1->Caption = "trying to connect...\n";
	TformatCtx pFormatCtx;

   //	connect(pFormatCtx);

   char* sourceUrl = new char[Form4->conSource->Text.Length() + 1];
	wcstombs(sourceUrl,Form4->conSource->Text.c_str(),Form4->conSource->Text.Length() + 1);
	int nRes = avformat_open_input(&pFormatCtx.m_ctx, sourceUrl, NULL, NULL);
	delete sourceUrl;
	if (nRes < 0) {// sometimes stuck here; not sure what the problem is
		// most likely it can't properly connect
		// stuck at line 755 (ret = ff_rtsp_connect(s);) in rtspdec.c

		//Form4->Label1->Caption = "couldn't connect\n";
		return "couldn't connect\n"; // Couldn't open file
	}

	/*
    try {
		connectTimeout(pFormatCtx);
    }
    catch(std::runtime_error& e) {
		Form4->Label1->Caption = e.what();

	}   */

	Form4->Label1->Caption = "connection successful\ntrying to find stream info...\n";
	//Form4->Label1->Caption = "trying to find stream info...\n";

	 // Retrieve stream information
	nRes = avformat_find_stream_info(pFormatCtx.m_ctx, NULL);
	if (nRes < 0) {
		//Form4->Label1->Caption = "Couldn't find stream information\n";
		return "Couldn't find stream information\n"; // Couldn't find stream information
	}
	Form4->Label1->Caption = "found stream information\n";

     // Find the first video stream
    int videoStreamIndex = -1;
    for (int i = 0; i < pFormatCtx.m_ctx->nb_streams; i++)
        if (pFormatCtx.m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
			Form4->Label1->Caption = "found a video stream\n";
            break;
        }
    if (videoStreamIndex == -1)
	{
		//Form4->Label1->Caption = "couldn't find a video stream\n";
		return "couldn't find a video stream\n";
	}

	TcodecCtx pCodecCtx;
    pCodecCtx.m_ctx = avcodec_alloc_context3(NULL);
    if (pCodecCtx.m_ctx == NULL) {
		//Form4->Label1->Caption = "codec context allocation error\n";
		return "codec context allocation error\n";
    }
	Form4->Label1->Caption = "codec context allocated\n";


	nRes = avcodec_parameters_to_context(pCodecCtx.m_ctx, pFormatCtx.m_ctx->streams[videoStreamIndex]->codecpar);

    if (nRes < 0)
	{
		//Form4->Label1->Caption = "couldn't convert codec parameters to context\n";
		return "couldn't convert codec parameters to context\n";
    }

    Form4->Label1->Caption = "codec parameters converted to context\n";

    // Find the decoder for the video stream
    AVCodec* pCodec = NULL;
    pCodec = const_cast <AVCodec*> (avcodec_find_decoder(pCodecCtx.m_ctx->codec_id));
	if (pCodec == NULL) {
		//Form4->Label1->Caption = stderr + "Unsupported codec!\n";
		//Form4->Label1->Caption = "Unsupported codec!\n";
		return "Unsupported codec!\n"; // Codec not found
	}
    Form4->Label1->Caption = "found the decoder\n";

    // Open codec
    AVDictionary* optionsDict = NULL;
    nRes = avcodec_open2(pCodecCtx.m_ctx, pCodec, &optionsDict);
	if (nRes < 0) {
		//Form4->Label1->Caption = "Could not open codec\n";
		return "Could not open codec\n"; // Could not open codec
    }
    Form4->Label1->Caption = "opened codec\n";


      // Allocate video frame (from source video)
    //AVFrame* pFrame = NULL;
	Tframe pFrame;
    pFrame.m_fr = av_frame_alloc();
	if (pFrame.m_fr == NULL) {
		//Form4->Label1->Caption = "video (source) frame allocation error\n";
		return "video (source) frame allocation error\n";
    }

    // Allocate an AVFrame structure (for conversion result)
    //AVFrame* pFrameRGB = NULL;
    Tframe pFrameRGB;
    pFrameRGB.m_fr = av_frame_alloc();
    if (pFrameRGB.m_fr == NULL) {
		//Form4->Label1->Caption = "frame (result) allocation error\n";
		return "frame (result) allocation error\n";
    }

	Form4->Label1->Caption = "allocated frames\n";

	// Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, 1);
    Tbuffer buff(numBytes);
	//uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

    int w, h;
	if (pCodecCtx.m_ctx->width > 640) {
	   w = 640;
	}
	else w = pCodecCtx.m_ctx->width;

	if (pCodecCtx.m_ctx->height > 360) {
	   h = 360;
	}
	else h = pCodecCtx.m_ctx->height;

    // allocate SwsContext (image scaling)
	struct SwsContext* sws_ctx = sws_getContext(pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, pCodecCtx.m_ctx->pix_fmt,
		w, h, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
		//pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws_ctx == NULL) {
		//Form4->Label1->Caption = "swsContext error\n";
		return "swsContext error\n";
	}


    // Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture



	av_image_fill_arrays(pFrameRGB.m_fr->data, pFrameRGB.m_fr->linesize, buff.m_buff, AV_PIX_FMT_RGB24, w, h, 1);
	//av_image_fill_arrays(pFrameRGB.m_fr->data, pFrameRGB.m_fr->linesize, buff.m_buff, AV_PIX_FMT_RGB24, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, 1);

	// Read frames and save first five frames to disk
    int i = 0;


    while (true) {
        TPacket packet;
        int nRes = av_read_frame(pFormatCtx.m_ctx, &packet.m_packet);
		if (nRes < 0)
            break;
        if (packet.m_packet.stream_index != videoStreamIndex)
            continue;
		avcodec_send_packet(pCodecCtx.m_ctx, &packet.m_packet);
        if (avcodec_receive_frame(pCodecCtx.m_ctx, pFrame.m_fr) == 0) {
            //    if (frameFinished) {
                // Convert the image from its native format to RGB
			sws_scale(sws_ctx, (uint8_t const* const*)pFrame.m_fr->data, pFrame.m_fr->linesize, 0,
                pCodecCtx.m_ctx->height, pFrameRGB.m_fr->data, pFrameRGB.m_fr->linesize);

			// Save the frame to disk
			//if (++i <= 5)
				  i++;
			showFrame(pFrameRGB.m_fr, w, h, i);

				//SaveFrame(pFrameRGB.m_fr, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, i);
		}
	}

	char *result = new char[18];
	std::stringstream strs;
	strs << i;
	std::string temp_str = strs.str();
	char const* c = temp_str.c_str();
	strcpy(result,"captured ");
	strcat(result,c);
	strcat(result," frames");

	return result;

}

//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm4::Button1Click(TObject *Sender)
{
  Form4->Label1->Caption = main();
}
//---------------------------------------------------------------------------

