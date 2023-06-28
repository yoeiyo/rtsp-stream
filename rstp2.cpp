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

using namespace std;

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
        cout << "Please provide source\n";
        return -1;
    }


    // Open video file
    //AVFormatContext* pFormatCtx = NULL;
    formatCtx pFormatCtx;
    int fun = avformat_open_input(&pFormatCtx.ctx, argv[1], NULL, NULL);
    if (fun != 0) {// sometimes stuck here; not sure what the problem is
        // most likely it can't properly connect
        // stuck at line 755 (ret = ff_rtsp_connect(s);) in rtspdec.c
        cout << "couldn't connect\n";
        return -1; // Couldn't open file
    }


      // Retrieve stream information
    fun = avformat_find_stream_info(pFormatCtx.ctx, NULL);
    if (fun < 0) {
        cout << "Couldn't find stream information\n";
        return -1; // Couldn't find stream information
    }
        

      // Dump information about file onto standard error
    av_dump_format(pFormatCtx.ctx, 0, argv[1], 0); //Print detailed information about the input or output format,
    //such as duration, bitrate, streams, container, programs, metadata, side data, codec and time base.


    // Find the first video stream
    int videoStream = -1;
    for (int i = 0; i < pFormatCtx.ctx->nb_streams; i++)
        if (pFormatCtx.ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            cout << "found a video stream\n";
            break;
        }
    if (videoStream == -1)
    {
        cout << "Didn't find a video stream\n";
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
   // AVCodecContext* pCodecCtx = NULL;
    codecCtx pCodecCtx;
    pCodecCtx.ctx = avcodec_alloc_context3(NULL);
    if (pCodecCtx.ctx == NULL) {
        cout << "codec context allocation error\n";
        return -1;
    }
        

    fun = avcodec_parameters_to_context(pCodecCtx.ctx, pFormatCtx.ctx->streams[videoStream]->codecpar);

    if (fun < 0)
    {
        cout << "couldn't convert codec parameters to context\n";
        return -1;
    }


    // Find the decoder for the video stream
    AVCodec* pCodec = NULL;
    pCodec = const_cast <AVCodec*> (avcodec_find_decoder(pCodecCtx.ctx->codec_id));
    if (pCodec == NULL) {
        cout << stderr << "Unsupported codec!\n";
        return -1; // Codec not found
    }

    // Open codec
    AVDictionary* optionsDict = NULL;
    fun = avcodec_open2(pCodecCtx.ctx, pCodec, &optionsDict);
    if (fun < 0) {
        cout << "Could not open codec\n";
        return -1; // Could not open codec
    }
        

      // Allocate video frame (from source video)
    AVFrame* pFrame = NULL;
    pFrame = av_frame_alloc();
    if (pFrame == NULL) {
        cout << "video (source) frame allocation error\n";
        return -1;
    }

    // Allocate an AVFrame structure (for conversion result)
    AVFrame* pFrameRGB = NULL;
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL) {
        cout << "frame (result) allocation error\n";
        return -1;
    }

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx.ctx->width, pCodecCtx.ctx->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

    // allocate SwsContext (image scaling)
    struct SwsContext* sws_ctx = NULL;
    sws_ctx =
        sws_getContext
        (
            pCodecCtx.ctx->width,
            pCodecCtx.ctx->height,
            pCodecCtx.ctx->pix_fmt,
            pCodecCtx.ctx->width,
            pCodecCtx.ctx->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL
        );

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx.ctx->width, pCodecCtx.ctx->height, 1);

    // Read frames and save first five frames to disk
    int i = 0;
    AVPacket packet;
    while (av_read_frame(pFormatCtx.ctx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            // Decode video frame
           // avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            avcodec_send_packet(pCodecCtx.ctx, &packet);

            // Did we get a video frame?
            if (avcodec_receive_frame(pCodecCtx.ctx, pFrame) == 0) {
                //    if (frameFinished) {
                    // Convert the image from its native format to RGB
                sws_scale
                (
                    sws_ctx,
                    (uint8_t const* const*)pFrame->data,
                    pFrame->linesize,
                    0,
                    pCodecCtx.ctx->height,
                    pFrameRGB->data,
                    pFrameRGB->linesize
                );

                // Save the frame to disk
                if (++i <= 5)
                    SaveFrame(pFrameRGB, pCodecCtx.ctx->width, pCodecCtx.ctx->height, i);
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_packet_unref(&packet);
    }

    // avcodec_free_context(&pCodecCtx);

     // Free the RGB image
    av_free(buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    //avcodec_close(pCodecCtx);

    // Close the input
    //avformat_close_input(&pFormatCtx);

    return 0;
}