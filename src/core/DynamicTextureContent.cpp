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

#include "DynamicTextureContent.h"

#include "DynamicTexture.h"
#include "serializationHelpers.h"
#include "Factories.h"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(DynamicTextureContent, "DynamicTextureContent")

DynamicTextureContent::DynamicTextureContent(const QString& uri)
    : Content(uri)
{}

CONTENT_TYPE DynamicTextureContent::getType()
{
    return CONTENT_TYPE_DYNAMIC_TEXTURE;
}

bool DynamicTextureContent::readMetadata()
{
    QFileInfo file( getURI( ));
    if (!file.exists() || !file.isReadable())
        return false;

    const DynamicTexture dynamicTexture(getURI());
    dynamicTexture.getDimensions( size_.rwidth(), size_.rheight( ));
    return true;
}

const QStringList& DynamicTextureContent::getSupportedExtensions()
{
    static QStringList extensions;

    if (extensions.empty())
    {
        extensions << "pyr";

        const QList<QByteArray>& imageFormats = QImageReader::supportedImageFormats();
        foreach( const QByteArray entry, imageFormats )
            extensions << entry;
    }

    return extensions;
}

void DynamicTextureContent::preRenderUpdate(Factories& factories, ContentWindowPtr, WallToWallChannel&)
{
    factories.getDynamicTextureFactory().getObject(getURI())->preRenderUpdate();
}

void DynamicTextureContent::postRenderUpdate(Factories& factories, ContentWindowPtr, WallToWallChannel&)
{
    if( blockAdvance_ )
        return;

    factories.getDynamicTextureFactory().getObject(getURI())->postRenderUpdate();
}
