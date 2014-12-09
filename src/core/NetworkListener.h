/*********************************************************************/
/* Copyright (c) 2011 - 2012, The University of Texas at Austin.     */
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

#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H

#include <QtNetwork/QTcpServer>

class PixelStreamDispatcher;
class PixelStreamWindowManager;
class CommandHandler;

/**
 * Listen to incoming PixelStream connections from dc::Stream clients.
 */
class NetworkListener : public QTcpServer
{
    Q_OBJECT

public:
    /** The default port number used for dc::Stream connections. */
    static const int defaultPortNumber_;

    /**
     * Create a new server listening for dc::Stream connections.
     * @param windowManager A manager to handle the creation of Stream windows.
     * @param port The port to listen on. Must be available.
     * @throw std::runtime_error if the server could not be started.
     */
    NetworkListener(PixelStreamWindowManager& windowManager,
                    int port = defaultPortNumber_);

    /** Destructor */
    ~NetworkListener();

    /** Get the command handler. */
    CommandHandler& getCommandHandler() const;

    /** Get the PixelStreamDispatcher. */
    PixelStreamDispatcher* getPixelStreamDispatcher() const;

protected:
    /** Re-implemented handling of connections from QTCPSocket. */
    void incomingConnection(int socketHandle) override;

private:
    PixelStreamWindowManager& windowManager_;
    PixelStreamDispatcher* pixelStreamDispatcher_;
    CommandHandler* commandHandler_;
};

#endif
