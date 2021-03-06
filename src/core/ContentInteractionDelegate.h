/*********************************************************************/
/* Copyright (c) 2013, EPFL/Blue Brain Project                       */
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

#ifndef CONTENTINTERACTIONDELEGATE_H
#define CONTENTINTERACTIONDELEGATE_H

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QObject>

class ContentWindow;
class DoubleTapGesture;
class PanGesture;
class PinchGesture;
class QTapGesture;
class QSwipeGesture;
class QTapAndHoldGesture;


class ContentInteractionDelegate
{
public:
    ContentInteractionDelegate(ContentWindow& contentWindow);
    virtual ~ContentInteractionDelegate();

    // Main entry point for gesture events
    void gestureEvent( QGestureEvent *event );

    // Virtual touch gestures
    virtual void tap( QTapGesture* gesture ) { Q_UNUSED(gesture) }
    virtual void doubleTap( DoubleTapGesture* gesture ) { Q_UNUSED(gesture) }
    virtual void pan( PanGesture* gesture ) { Q_UNUSED(gesture) }
    virtual void swipe( QSwipeGesture*gesture ) { Q_UNUSED(gesture) }
    virtual void pinch( PinchGesture* gesture ) { Q_UNUSED(gesture) }
    //virtual void tapAndHold( QTapAndHoldGesture* gesture ) { Q_UNUSED(gesture) }

    // Keyboard + Mouse input
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) { Q_UNUSED( event ) }
    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event ) { Q_UNUSED( event ) }
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) { Q_UNUSED( event ) }
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) { Q_UNUSED( event ) }
    virtual void wheelEvent( QGraphicsSceneWheelEvent* event ) { Q_UNUSED( event ) }
    virtual void keyPressEvent( QKeyEvent* event ) { Q_UNUSED( event ) }
    virtual void keyReleaseEvent( QKeyEvent* event ) { Q_UNUSED( event ) }

protected:
    ContentWindow& contentWindow_;

    double adaptZoomFactor( double pinchGestureScaleFactor );

private:
    // Touch gestures when ContentWindow is not in interaction mode
    void doubleTapUnselected( DoubleTapGesture* gesture );
    void tapAndHoldUnselected( QTapAndHoldGesture* gesture );
    void panUnselected( PanGesture* gesture );
    void pinchUnselected( PinchGesture* gesture );

};

#endif // CONTENTINTERACTIONDELEGATE_H
