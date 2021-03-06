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

#include "WallConfiguration.h"

#include <QtXmlPatterns>
#include <stdexcept>

WallConfiguration::WallConfiguration(const QString &filename, const int processIndex)
    : Configuration(filename)
    , screenCountForCurrentProcess_(0)
{
    loadWallSettings(processIndex);
}

void WallConfiguration::loadWallSettings(const int processIndex)
{
    assert(processIndex > 0 && "WallConfiguration::loadWallSettings is only valid for processes of rank > 0");

    const int xpathIndex = processIndex; // xpath index also starts from 1

    QXmlQuery query;
    if(!query.setFocus(QUrl(filename_)))
        throw std::runtime_error("Invalid configuration file: " + filename_.toStdString());

    QString queryResult;

    // get host
    query.setQuery( QString("string(//process[%1]/@host)").arg(xpathIndex) );
    if (query.evaluateTo(&queryResult))
        host_ = queryResult.remove(QRegExp("[\\n\\t\\r]"));

    // get display (optional attribute)
    query.setQuery( QString("string(//process[%1]/@display)").arg(xpathIndex) );
    if(query.evaluateTo(&queryResult))
        display_ = queryResult.remove(QRegExp("[\\n\\t\\r]"));
    else
        display_ = QString("default (:0)"); // the default

    // get number of tiles for my process
    query.setQuery( QString("string(count(//process[%1]/screen))").arg(xpathIndex) );
    if (query.evaluateTo(&queryResult))
        screenCountForCurrentProcess_ = queryResult.toInt();

    // populate parameters for each screen
    for(int i=1; i<=screenCountForCurrentProcess_; i++)
    {
        QPoint screenPosition;

        query.setQuery( QString("string(//process[%1]/screen[%2]/@x)").arg(xpathIndex).arg(i) );
        if(query.evaluateTo(&queryResult))
            screenPosition.setX(queryResult.toInt());

        query.setQuery( QString("string(//process[%1]/screen[%2]/@y)").arg(xpathIndex).arg(i) );
        if(query.evaluateTo(&queryResult))
            screenPosition.setY(queryResult.toInt());

        screenPosition_.push_back(screenPosition);

        QPoint screenIndex;

        query.setQuery( QString("string(//process[%1]/screen[%2]/@i)").arg(xpathIndex).arg(i) );
        if(query.evaluateTo(&queryResult))
            screenIndex.setX(queryResult.toInt());

        query.setQuery( QString("string(//process[%1]/screen[%2]/@j)").arg(xpathIndex).arg(i) );
        if(query.evaluateTo(&queryResult))
            screenIndex.setY(queryResult.toInt());

        screenGlobalIndex_.push_back(screenIndex);
    }
}

const QString& WallConfiguration::getHost() const
{
    return host_;
}

const QString& WallConfiguration::getDisplay() const
{
    return display_;
}

int WallConfiguration::getScreenCount() const
{
    return screenCountForCurrentProcess_;
}

const QPoint& WallConfiguration::getScreenPosition(int screenIndex) const
{
    return screenPosition_.at(screenIndex);
}

const QPoint& WallConfiguration::getGlobalScreenIndex(int screenIndex) const
{
    return screenGlobalIndex_.at(screenIndex);
}
