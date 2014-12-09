/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#ifndef FFMPEGMOVIE_H
#define FFMPEGMOVIE_H

// required for FFMPEG includes below, specifically for the Linux build
#ifdef __cplusplus
    #ifndef __STDC_CONSTANT_MACROS
        #define __STDC_CONSTANT_MACROS
    #endif

    #ifdef _STDINT_H
        #undef _STDINT_H
    #endif

    #include <stdint.h>
#endif

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/error.h>
    #include <libavutil/mathematics.h>
}

#include <boost/date_time/posix_time/posix_time.hpp>

#include <QString>

class FFMPEGVideoFrameConverter;

/**
 * Read and play movies using the FFMPEG library.
 */
class FFMPEGMovie
{
public:
    /**
     * Constructor.
     * @param uri: the movie file to open.
     */
    FFMPEGMovie(const QString& uri);

    /** Destructor */
    ~FFMPEGMovie();

    /** Is the movie valid. */
    bool isValid() const;

    /** Get the frame width. */
    unsigned int getWidth() const;
    /** Get the frame height. */
    unsigned int getHeight() const;

    /**
     * Get a pointer to the iamge data.
     * The buffer has format GL_RGBA, GL_UNSIGNED_BYTE and size getWidth()*getHeight() bytes.
     * @return Pointer to the data of the last frame decoded.
     */
    const void* getData() const;

    /**
     * Update the internal timestamp to play the movie.
     * @param timestamp The desired timestamp.
     * @param skipDecoding Only update the timestamp without reading or decoding the movie.
     */
    void update(const boost::posix_time::time_duration timestamp, const bool skipDecoding);

    /**
     * Is there a new frame available.
     * @return true if a new frame was decoded as a result of the last call to
     *         update() or jumpTo()
     * @see update()
     * @see jumpTo()
     */
    bool isNewFrameAvailable() const;

    /** Set looping behaviour when reaching the end of the file. */
    void setLoop(const bool loop);

    /** Get the movie duration in seconds. May be unavailable for some movies. */
    double getDuration() const;

    /** Get the current timestamp. */
    boost::posix_time::time_duration getTimestamp() const;

    /**
     * Jump to a random position in the movie.
     * @param timePosInSeconds The desired position in seconds
     * @return true on success
     */
    bool jumpTo(const double timePosInSeconds);

private:
    // FFMPEG
    AVFormatContext * avFormatContext_;    // AV Format information from the file header
    AVCodecContext * videoCodecContext_;   // shortcut to videostream_->codec; don't free
    AVFrame * avFrame_;                    // Frame for decoding
    AVStream * videoStream_;               // shortcut to avFormatContext_->streams[streamIdx_]; don't free

    FFMPEGVideoFrameConverter * videoFrameConverter_; // Convert decoded frames to RGBA format

    // used for seeking
    int64_t den2_;
    int64_t num2_;
    int64_t numFrames_;
    double frameDurationInSeconds_;

    boost::posix_time::time_duration timePosition_;

    // Internal
    bool loop_;
    int frameDecodingComplete_;
    bool isValid_;
    bool skippedFrames_;

    // Public status
    bool newFrameAvailable_;

    /** Init the global FFMPEG context. */
    static void initGlobalState();

    bool open(const QString& uri);

    bool createAvFormatContext(const QString& uri);
    void releaseAvFormatContext();

    bool findVideoStream();

    bool openVideoStreamDecoder();
    void closeVideoStreamDecoder() const;

    bool readVideoFrame();
    bool seekToNearestFullframe(const int64_t frameIndex);
    void clampTimePosition();
    int64_t getTimestampForFrameIndex(const int64_t frameIndex) const;
    bool decodeVideoFrame(AVPacket& packet);
    bool convertVideoFrame();
    void rewind();
    bool isVideoStream(const AVPacket& packet) const;
    void generateSeekingParameters();
};

#endif // FFMPEGMOVIE_H
