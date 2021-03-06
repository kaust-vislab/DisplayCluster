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

#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QDialog>

#include "types.h"

class MasterConfiguration;
class QLabel;

/**
 * Simple widget to edit and save background settings.
 */
class BackgroundWidget : public QDialog
{
    Q_OBJECT

public:
    /**
     * Create a BackgroundWidget
     * @param configuration The configuration in which to read and save
     *                      background settings.
     * @param parent An optional parent widget
     */
    BackgroundWidget( MasterConfiguration& configuration, QWidget* parent = 0 );

signals:
    /** Emitted when the user selected a different color */
    void backgroundColorChanged( QColor color );

    /** Emitted when the user selected a different background */
    void backgroundContentChanged( ContentPtr content );

public slots:
    /** Store the new settings and close the widget */
    void accept() override;

    /** Revert to the previous settings and close the widget */
    void reject() override;

private slots:
    void chooseColor();
    void openBackgroundContent();
    void removeBackground();

private:
    MasterConfiguration& configuration_;

    QLabel *colorLabel_;
    QLabel *backgroundLabel_;

    QColor previousColor_;
    QString previousBackgroundURI_;
};

#endif // BACKGROUNDWIDGET_H
