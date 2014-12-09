/*********************************************************************/
/* Copyright (c) 2013-2014, EPFL/Blue Brain Project                  */
/*                     Daniel.Nachbaur@epfl.ch                       */
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

#ifndef DCSTREAMSENDWORKER_H
#define DCSTREAMSENDWORKER_H

// needed for future.hpp with Boost 1.41
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <boost/thread/future.hpp>
#include <boost/thread/thread.hpp>
#include <deque>

#include "Stream.h" // Stream::Future

namespace dc
{

class StreamPrivate;
struct ImageWrapper;

/**
 * Worker class that is used to send images that are pushed to a worker queue.
 */
class StreamSendWorker
{
public:
    /** Create a new stream worker associated to an existing stream object. */
    StreamSendWorker( StreamPrivate& stream );

    ~StreamSendWorker();

    /** Enqueue an image to be send during the execution of run(). */
    Stream::Future enqueueImage( const ImageWrapper& image );

private:
    /** Starts asynchronous sending of queued images. */
    void run_();

    /** Stop the worker and clear any pending image send requests. */
    void stop_();

    typedef boost::promise< bool > Promise;
    typedef boost::shared_ptr< Promise > PromisePtr;
    typedef std::pair< PromisePtr, ImageWrapper > Request;

    StreamPrivate& stream_;
    std::deque< Request > requests_;
    boost::mutex mutex_;
    boost::condition condition_;
    bool running_;
    boost::thread thread_;
};

}
#endif // DCSTREAMPRIVATE_H
