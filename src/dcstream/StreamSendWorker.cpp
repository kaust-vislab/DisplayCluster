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

#include "StreamSendWorker.h"

#include "StreamPrivate.h"

namespace dc
{

StreamSendWorker::StreamSendWorker( StreamPrivate& stream )
    : stream_( stream )
    , running_( true )
    , thread_( boost::bind( &StreamSendWorker::run_, this ))
{}

StreamSendWorker::~StreamSendWorker()
{
    stop_();
}

void StreamSendWorker::run_()
{
    boost::mutex::scoped_lock lock( mutex_ );
    while( true )
    {
        while( requests_.empty() && running_ )
            condition_.wait( lock );
        if( !running_ )
            break;

        const Request& request = requests_.back();
        request.first->set_value( stream_.send( request.second ) && stream_.finishFrame( ));
        requests_.pop_back();
    }
}

void StreamSendWorker::stop_()
{
    {
        boost::mutex::scoped_lock lock( mutex_ );
        running_ = false;
        condition_.notify_all();
    }

    thread_.join();
    requests_.clear();
}

Stream::Future StreamSendWorker::enqueueImage( const ImageWrapper& image )
{
    boost::mutex::scoped_lock lock( mutex_ );
    PromisePtr promise( new Promise );
    requests_.push_back( Request( promise, image ));
    condition_.notify_all();
    return promise->get_future();
}

}
