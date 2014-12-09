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

#include "ContentWindowRenderer.h"

#include "globals.h"
#include "configuration/Configuration.h"
#include "Options.h"
#include "ContentWindow.h"
#include "Content.h"
#include "GLWindow.h"
#include "Factories.h"

#define CONTEXT_VIEW_REL_SIZE       0.25f
#define CONTEXT_VIEW_PADDING        0.02f
#define CONTEXT_VIEW_DELTA_Z        0.001f
#define CONTEXT_VIEW_ALPHA          0.5f
#define CONTEXT_VIEW_BORDER_WIDTH   5.f
#define WINDOW_BORDER_WIDTH_PIXELS  5.f

ContentWindowRenderer::ContentWindowRenderer(FactoriesPtr factories)
    : factories_(factories)
    , showWindowBorders_(true)
    , showZoomContext_(false)
    , showSegmentBorders_(false)
    , showSegmentStatistics_(false)
{
    quad_.setEnableTexture(false);
}

void ContentWindowRenderer::render()
{
    if(!window_)
        return;

    renderContent();

    if(showWindowBorders_ || window_->selected())
        renderWindowBorder();
}

void ContentWindowRenderer::setContentWindow(ContentWindowPtr window)
{
    window_ = window;
}

void ContentWindowRenderer::setShowWindowBorders(const bool show)
{
    showWindowBorders_ = show;
}

void ContentWindowRenderer::setShowZoomContext(const bool show)
{
    showZoomContext_ = show;
}

void ContentWindowRenderer::setPixelStreamOptions(const bool showSegmentBorders,
                                                  const bool showSegmentStatistics)
{
    showSegmentBorders_ = showSegmentBorders;
    showSegmentStatistics_ = showSegmentStatistics;
}

void ContentWindowRenderer::renderWindowBorder()
{
    const float horizontalBorder = WINDOW_BORDER_WIDTH_PIXELS /
            (float)g_configuration->getTotalHeight();

    const float verticalBorder = horizontalBorder /
            g_configuration->getAspectRatio();

    glPushAttrib(GL_CURRENT_BIT);

    if(window_->selected())
        glColor4f(1,0,0,1);
    else
        glColor4f(1,1,1,1);

    const QRectF winCoord = window_->getCoordinates();

    drawQuad(QRectF(winCoord.x() - verticalBorder,
                    winCoord.y() - horizontalBorder,
                    winCoord.width() + 2.f*verticalBorder,
                    winCoord.height() + 2.f*horizontalBorder));

    glPopAttrib();
}

void ContentWindowRenderer::renderContent()
{
    const QRectF winCoord = window_->getCoordinates();
    const QRectF texCoord = getTexCoord();

    // transform to a normalize coordinate system so the content
    // can be rendered at (x,y,w,h) = (0,0,1,1)
    glPushMatrix();

    glTranslatef(winCoord.x(), winCoord.y(), 0.f);
    glScalef(winCoord.width(), winCoord.height(), 1.f);

    FactoryObjectPtr object = factories_->getFactoryObject(window_->getContent());

    PixelStream* pixelStream = dynamic_cast< PixelStream* >(object.get());
    if(pixelStream)
        pixelStream->setRenderingOptions(showSegmentBorders_, showSegmentStatistics_);

    object->render(texCoord);

    if(showZoomContext_ && window_->getZoom() > 1.)
        renderContextView(object, texCoord);

    glPopMatrix();
}

QRectF ContentWindowRenderer::getTexCoord() const
{
    double centerX, centerY;
    window_->getCenter(centerX, centerY);

    const double zoom = window_->getZoom();

    return QRectF(centerX - 0.5/zoom, centerY - 0.5/zoom, 1./zoom, 1./zoom);
}

void ContentWindowRenderer::renderContextView(FactoryObjectPtr object,
                                              const QRectF& texCoord)
{
    const QRectF unitRect(0.f, 0.f, 1.f, 1.f);

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
    glPushMatrix();

    // position at lower left
    glTranslatef(CONTEXT_VIEW_PADDING,
                 1.f - CONTEXT_VIEW_REL_SIZE - CONTEXT_VIEW_PADDING,
                 CONTEXT_VIEW_DELTA_Z);
    glScalef(CONTEXT_VIEW_REL_SIZE, CONTEXT_VIEW_REL_SIZE, 1.f);

    // render border rectangle
    glColor4f(1,1,1,1);
    drawQuadBorder(unitRect, CONTEXT_VIEW_BORDER_WIDTH);

    // render the factory object (full view)
    glTranslatef(0.f, 0.f, CONTEXT_VIEW_DELTA_Z);
    object->render(unitRect);

    glTranslatef(0.f, 0.f, CONTEXT_VIEW_DELTA_Z);
    drawQuadBorder(texCoord, CONTEXT_VIEW_BORDER_WIDTH);

    // draw context rectangle blended
    glTranslatef(0.f, 0.f, CONTEXT_VIEW_DELTA_Z);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.f, 1.f, 1.f, CONTEXT_VIEW_ALPHA);
    drawQuad(texCoord);

    glPopMatrix();
    glPopAttrib();
}

void ContentWindowRenderer::drawQuad(const QRectF& coord)
{
    glPushMatrix();

    glTranslatef(coord.x(), coord.y(), 0);
    glScalef(coord.width(), coord.height(), 1.f);

    quad_.setRenderMode(GL_QUADS);
    quad_.render();

    glPopMatrix();
}

void ContentWindowRenderer::drawQuadBorder(const QRectF& coord, const float width)
{
    glPushMatrix();

    glTranslatef(coord.x(), coord.y(), 0);
    glScalef(coord.width(), coord.height(), 1.f);

    glLineWidth(width);
    quad_.setRenderMode(GL_LINE_LOOP);
    quad_.render();

    glPopMatrix();
}
