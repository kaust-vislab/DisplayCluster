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

#include "WebkitAuthenticationHelper.h"

#include "log.h"

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QFile>
#include <QTextStream>

#define HTTP_LOGIN_PAGE_URL             ":/html/login-form.htm"
#define HTTP_FORM_SELECTOR              "form[name=http-login-form]"
#define HTTP_WEBSITE_URL_ELEM_SELECTOR  "p[id=website_url]"
#define HTTP_USERNAME_INPUT_NAME        "username"
#define HTTP_PASSWORD_INPUT_NAME        "password"


WebkitAuthenticationHelper::WebkitAuthenticationHelper(QWebView &webView)
    : webView_(webView)
    , userHasInputNewCredentials_(false)
{
    connect(webView_.page()->networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(handleAuthenticationRequest(QNetworkReply*,QAuthenticator*)));
}

void WebkitAuthenticationHelper::handleAuthenticationRequest(QNetworkReply*, QAuthenticator* authenticator)
{
    // This method is only called by the QNetworkAccessManager when authentication has failed
    if (userHasInputNewCredentials_)
    {
        sendCredentials(authenticator);
        userHasInputNewCredentials_ = false;
    }
    else
    {
        connect(&webView_, SIGNAL(loadFinished(bool)), this, SLOT(errorPageFinishedLoading(bool)));
    }
}

void WebkitAuthenticationHelper::sendCredentials(QAuthenticator* authenticator) const
{
    authenticator->setUser(username_);
    authenticator->setPassword(password_);
}

void WebkitAuthenticationHelper::errorPageFinishedLoading(const bool ok)
{
    if ( !ok )
        return;

    disconnect(&webView_, SIGNAL(loadFinished(bool)), this, SLOT(errorPageFinishedLoading(bool)));

    displayLoginPage();
    registerLoginFormCallbacks();
}

void WebkitAuthenticationHelper::displayLoginPage()
{
    // Note: Also setting target URL in webview for future call to reload()
    webView_.setHtml(readQrcFile(HTTP_LOGIN_PAGE_URL), webView_.url());

    // Display target url
    QWebElement websiteUrlElement = webView_.page()->mainFrame()->documentElement().findFirst(HTTP_WEBSITE_URL_ELEM_SELECTOR);
    websiteUrlElement.setInnerXml(webView_.url().toString());
}

void WebkitAuthenticationHelper::registerLoginFormCallbacks()
{
    QWebFrame* frame = webView_.page()->mainFrame();
    frame->addToJavaScriptWindowObject("authenticationHelper", this);

    QWebElement form = frame->findFirstElement(HTTP_FORM_SELECTOR);
    form.setAttribute("onsubmit", QString("authenticationHelper.loginFormSubmitted();"));

    QWebElementCollection inputs = form.findAll("input");
    foreach(QWebElement input, inputs)
    {
        input.setAttribute("onchange", QString("authenticationHelper.loginFormInputChanged(this.name, this.value);"));
    }
}

void WebkitAuthenticationHelper::loginFormInputChanged(const QString& inputName, const QString& inputValue)
{
    if (inputName == HTTP_USERNAME_INPUT_NAME)
    {
        username_ = inputValue;
        userHasInputNewCredentials_ = true;
    }
    else if (inputName == HTTP_PASSWORD_INPUT_NAME)
    {
        password_ = inputValue;
        userHasInputNewCredentials_ = true;
    }
}

void WebkitAuthenticationHelper::loginFormSubmitted()
{
    webView_.reload();
}

QString WebkitAuthenticationHelper::readQrcFile(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        return stream.readAll();
    }
    put_flog(LOG_ERROR, "Qt Resource not found: %s", filename.toLocal8Bit().constData());
    return "";
}
