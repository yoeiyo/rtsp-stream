extern "C"
{
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/avconfig.h>
#include <libavutil/avutil.h>
}

#include <iostream>
#include <stdio.h>

#include "formatCtx.h"
#include "codecCtx.h"
#include "frame.h"
#include "buffer.h"
#include "TPacket.h"

//using namespace std;

void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame) {
    FILE* pFile;
    char szFilename[32];

    // Open file
    sprintf_s(szFilename, "frame%d.ppm", iFrame);
    fopen_s(&pFile, szFilename, "wb");
    if (pFile == NULL)
    {
        printf("file wasn't saved\n");
        return;

    }
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (int y = 0; y < height; y++)
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
    printf("file was saved and closed\n");
}

int main(int argc, char* argv[]) {
    

    if (argc < 2) {
        std::cout << "Please provide source\n";
        return -1;
    }

    std::cout << "trying to connect...\n";

    // Open video file
    //AVFormatContext* pFormatCtx = NULL;
    TformatCtx pFormatCtx;
    int nRes = avformat_open_input(&pFormatCtx.m_ctx, argv[1], NULL, NULL);
    if (nRes < 0) {// sometimes stuck here; not sure what the problem is
        // most likely it can't properly connect
        // stuck at line 755 (ret = ff_rtsp_connect(s);) in rtspdec.c

        std::cout << "couldn't connect\n";
        return -1; // Couldn't open file
    }
    std::cout << "connection successful\n";
    std::cout << "trying to find stream info...\n";


      // Retrieve stream information
    nRes = avformat_find_stream_info(pFormatCtx.m_ctx, NULL);
    if (nRes < 0) {
        std::cout << "Couldn't find stream information\n";
        return -1; // Couldn't find stream information
    }
    std::cout << "found stream information\n";
        

      // Dump information about file onto standard error
    av_dump_format(pFormatCtx.m_ctx, 0, argv[1], 0); //Print detailed information about the input or output format,
    //such as duration, bitrate, streams, container, programs, metadata, side data, codec and time base.


    // Find the first video stream
    int videoStreamIndex = -1;
    for (int i = 0; i < pFormatCtx.m_ctx->nb_streams; i++)
        if (pFormatCtx.m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            std::cout << "found a video stream\n";
            break;
        }
    if (videoStreamIndex == -1)
    {
        std::cout << "Didn't find a video stream\n";
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
   // AVCodecContext* pCodecCtx = NULL;
    TcodecCtx pCodecCtx;
    pCodecCtx.m_ctx = avcodec_alloc_context3(NULL);
    if (pCodecCtx.m_ctx == NULL) {
        std::cout << "codec context allocation error\n";
        return -1;
    }
    std::cout << "codec context allocated\n";
        

    nRes = avcodec_parameters_to_context(pCodecCtx.m_ctx, pFormatCtx.m_ctx->streams[videoStreamIndex]->codecpar);

    if (nRes < 0)
    {
        std::cout << "couldn't convert codec parameters to context\n";
        return -1;
    }

    std::cout << "codec parameters converted to context\n";

    // Find the decoder for the video stream
    AVCodec* pCodec = NULL;
    pCodec = const_cast <AVCodec*> (avcodec_find_decoder(pCodecCtx.m_ctx->codec_id));
    if (pCodec == NULL) {
        std::cout << stderr << "Unsupported codec!\n";
        return -1; // Codec not found
    }
    std::cout << "found the decoder\n";

    // Open codec
    AVDictionary* optionsDict = NULL;
    nRes = avcodec_open2(pCodecCtx.m_ctx, pCodec, &optionsDict);
    if (nRes < 0) {
        std::cout << "Could not open codec\n";
        return -1; // Could not open codec
    }
    std::cout << "opened codec\n";
        

      // Allocate video frame (from source video)
    //AVFrame* pFrame = NULL;
    Tframe pFrame;
    pFrame.m_fr = av_frame_alloc();
    if (pFrame.m_fr == NULL) {
        std::cout << "video (source) frame allocation error\n";
        return -1;
    }

    // Allocate an AVFrame structure (for conversion result)
    //AVFrame* pFrameRGB = NULL;
    Tframe pFrameRGB;
    pFrameRGB.m_fr = av_frame_alloc();
    if (pFrameRGB.m_fr == NULL) {
        std::cout << "frame (result) allocation error\n";
        return -1;
    }

    std::cout << "allocated frames\n";

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, 1);
    Tbuffer buff(numBytes);
    //uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

    // allocate SwsContext (image scaling)
    struct SwsContext* sws_ctx = sws_getContext(pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, pCodecCtx.m_ctx->pix_fmt, 
        pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws_ctx == NULL) {
        std::cout << "swsContext error\n";
        return -1;
    }


    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture

    av_image_fill_arrays(pFrameRGB.m_fr->data, pFrameRGB.m_fr->linesize, buff.m_buff, AV_PIX_FMT_RGB24, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, 1);

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
            if (++i <= 5)
                //  i++;
                SaveFrame(pFrameRGB.m_fr, pCodecCtx.m_ctx->width, pCodecCtx.m_ctx->height, i);
        }

    }

    return 0;
}