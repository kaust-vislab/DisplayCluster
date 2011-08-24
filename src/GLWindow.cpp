#include "GLWindow.h"
#include "main.h"
#include "Content.h"
#include <QtOpenGL>

GLWindow::GLWindow()
{

}

GLWindow::~GLWindow()
{

}

void GLWindow::initializeGL()
{

}

void GLWindow::paintGL()
{
    double border = 0.0025;
    
    setView(width(), height());

    std::vector<boost::shared_ptr<Content> > contents = g_displayGroup.getContents();

    for(unsigned int i=0; i<contents.size(); i++)
    {
        double x,y,w,h;
        contents[i]->getCoordinates(x,y,w,h);

        QRectF rect(x,y,w,h);

        glEnable(GL_TEXTURE_2D);
        drawTexture(rect, textureFactory_.getTexture(contents[i]->getURI()));

        glDisable(GL_TEXTURE_2D);
        glColor4f(1,1,1,1);
        drawRectangle(x-border,y-border,w+2.*border,h+2.*border);
    }

    // continuously synchronize and update
    g_displayGroup.synchronizeContents();
    update();
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport (0, 0, width, height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    update();
}

void GLWindow::setView(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // invert y-axis to put origin at lower-left corner
    glScalef(1.,-1.,1.);

    // compute view bounds
    double left, right, bottom, top;

    if(g_mpiRank == 0)
    {
        left = 0.;
        right = 1.;
        bottom = 0.;
        top = 1.;
    }
    else
    {
        // tiled display parameters
        double tileI = (double)g_configuration->getTileI();
        double numTilesWidth = (double)g_configuration->getNumTilesWidth();
        double screenWidth = (double)g_configuration->getScreenWidth();
        double mullionWidth = (double)g_configuration->getMullionWidth();

        double tileJ = (double)g_configuration->getTileJ();
        double numTilesHeight = (double)g_configuration->getNumTilesHeight();
        double screenHeight = (double)g_configuration->getScreenHeight();
        double mullionHeight = (double)g_configuration->getMullionHeight();

        // border calculations
        left = tileI / numTilesWidth * ( numTilesWidth * screenWidth ) + tileI * mullionWidth;
        right = left + screenWidth;
        bottom = tileJ / numTilesHeight * ( numTilesHeight * screenHeight ) + tileJ * mullionHeight;
        top = bottom + screenHeight;

        // normalize to 0->1
        double totalWidth = numTilesWidth * screenWidth + (numTilesWidth - 1.) * mullionWidth;
        double totalHeight = numTilesHeight * screenHeight + (numTilesHeight - 1.) * mullionHeight;

        left /= totalWidth;
        right /= totalWidth;
        bottom /= totalHeight;
        top /= totalHeight;
    }

    gluOrtho2D(left, right, bottom, top);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();	

    glClearColor(0,0,0,0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable depth testing; disable lighting
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void GLWindow::push2DMode()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width(), 0, height());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void GLWindow::pop2DMode()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void GLWindow::drawRectangle(double x, double y, double w, double h)
{
    glBegin(GL_QUADS);

    glVertex2d(x,y);
    glVertex2d(x+w,y);
    glVertex2d(x+w,y+h);
    glVertex2d(x,y+h);

    glEnd();
}