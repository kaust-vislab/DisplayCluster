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

#ifndef FACTORIES_H
#define FACTORIES_H

#include "config.h"

#ifndef Q_MOC_RUN // See: https://bugreports.qt-project.org/browse/QTBUG-22829
#include "Factory.hpp"
#endif

#include "Texture.h"
#include "DynamicTexture.h"
#if ENABLE_PDF_SUPPORT
#include "PDF.h"
#endif
#include "SVG.h"
#include "Movie.h"
#include "PixelStream.h"

#include <QObject>

/**
 * A set of Factory<T> for all valid ContentTypes.
 *
 * It is used on Wall processes to map Content objects received from the
 * master application to FactoryObjects which hold the actual data.
 * It implements a basic garbage collection strategy for FactoryObjects
 * that are no longer referenced/accessed.
 */
class Factories : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param func the callback function when a new object in a factory was created
     */
    Factories(const Factory<FactoryObject>::NewObjectFunc& func);

    /**
     * Get the factory object associated to a given Content.
     *
     * If the object does not exist, it is created.
     * Objects not accessed during two consecutive frames are deleted using
     * a garbage collection mechanism.
     * @see postRenderUpdate()
     */
    FactoryObjectPtr getFactoryObject(ContentPtr content);

    /** Clear all Factories (useful on shutdown). */
    void clear();

    /** Update the objects before rendering. */
    void preRenderUpdate(DisplayGroup& displayGroup, WallToWallChannel& wallChannel);

    /** Update the objects after rendering and garbarge-collect unused ones. */
    void postRenderUpdate(DisplayGroup& displayGroup, WallToWallChannel& wallChannel);

    //@{
    /** Getters for specific Factory types. */
    Factory<Texture> & getTextureFactory();
    Factory<DynamicTexture> & getDynamicTextureFactory();
#if ENABLE_PDF_SUPPORT
    Factory<PDF> & getPDFFactory();
#endif
    Factory<SVG> & getSVGFactory();
    Factory<Movie> & getMovieFactory();
    Factory<PixelStream> & getPixelStreamFactory();
    //@}

public slots:
    /**
     * Update a PixelStream with the given frame.
     *
     * If the PixelStream for the frame uri does not exist, it is created.
     * The object accessed or created will not be garbage collected this frame.
     * @see clearStaleFactoryObjects()
     */
    void updatePixelStream(PixelStreamFramePtr frame);

private:
    /**
     * Garbarge-collect unused objects.
     *
     * Only call this function once per frame.
     * This will delete all FactoryObjects which have not been accessed since
     * this method was last called.
     */
    void clearStaleFactoryObjects();

    uint64_t frameIndex_;

    Factory<Texture> textureFactory_;
    Factory<DynamicTexture> dynamicTextureFactory_;
#if ENABLE_PDF_SUPPORT
    Factory<PDF> pdfFactory_;
#endif
    Factory<SVG> svgFactory_;
    Factory<Movie> movieFactory_;
    Factory<PixelStream> pixelStreamFactory_;
};

#endif // FACTORIES_H
