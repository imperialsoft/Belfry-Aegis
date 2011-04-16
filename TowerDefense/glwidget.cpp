#include "glwidget.h"
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}
//**************************************************************************************
//OpenGL Arrays
//**************************************************************************************
GLfloat color[]={1.0,1.0,1.0};
GLfloat scale[]={1.0,1.0,1.0};
GLfloat translate[]={0.0,0.0,0.0};
GLfloat rotate[]={0.0,0.0,0.0};
GLdouble eye[]={0.0,0.0,5.0};
GLdouble lookAt[]={0.0,0.0,0.0};
GLdouble lookUp[]={0.0,1.0,0.0};
GLdouble perspective[] = {100.0,1.0,10.0};
GLfloat light0_pos[]={0.0,20.0,10.0,1.0};
GLfloat diffuse0[]={1.0,1.0,1.0,1.0};
GLfloat ambient0[]={0.0,0.0,0.0,1.0};
GLfloat specular0[]={1.0,1.0,1.0,1.0};
//**************************************************************************************
//OpenGL Functions
//**************************************************************************************
void GLWidget::initializeGL()
{
    //Clear the screen with black
    glClearColor (0.0, 0.0, 0.0, 0.0);

    //Smoothly shade objects
    glShadeModel (GL_SMOOTH);

    //Eliminate unseen objects
    glEnable(GL_DEPTH_TEST);

    //Setup the light sources
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glLoadIdentity();
}

void GLWidget::paintGL()
{
    //Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set the color
    glColor3f(color[0],color[1],color[2]);

    //Clear the modelview matrix
    glLoadIdentity();

    //Set the eye, point of focus, and up direction
    gluLookAt(eye[0],eye[1],eye[2],lookAt[0],lookAt[1],lookAt[2],lookUp[0],lookUp[1],lookUp[2]);

    //Set the Perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    windowSize = size();
    gluPerspective(perspective[0],windowSize.width()/windowSize.height(),perspective[1],perspective[2]);
    glMatrixMode(GL_MODELVIEW);

    //glPushMatrix();
    //Set the scaling value
    //glScalef (scale[0], scale[1], scale[2]);

    //Set the translation values
    //glTranslatef(translate[0],translate[1],translate[2]);

    //Set the rotation values
    //glRotatef(rotate[0],1.0,0.0,0.0);
    //glRotatef(rotate[1],0.0,1.0,0.0);
    //glRotatef(rotate[2],0.0,0.0,1.0);
    //glRotatef(vectorRotate[0],vectorRotate[1],vectorRotate[2],vectorRotate[3]);
    //glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
    //glPopMatrix();
    //loadImages();
    glBegin(GL_POLYGON);
        glVertex3f(-1.0,1.0,0.0);
        glVertex3f(1.0,1.0,0.0);
        glVertex3f(1.0,0.0,0.0);
        glVertex3f(-1.0,0.0,0.0);
    glEnd();
    glFlush();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);//Set the viewable area
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    windowSize = size();
    gluPerspective(perspective[0],windowSize.width()/windowSize.height(),perspective[1],perspective[2]);
    glMatrixMode(GL_MODELVIEW);//Always set the MatrixMode back to maintain state consistency
}

//**************************************************************************************
//Qt Size Hints
//**************************************************************************************
QSize GLWidget::minimumSizeHint() const
{
        return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
        return QSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}
//**************************************************************************************
//GLWidget Slots
//**************************************************************************************
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        //Move Forward
    case Qt::Key_W:
        eye[2]-=0.1;
        updateGL();
        break;
        //Move Back
    case Qt::Key_S:
        eye[2]+=0.1;
        updateGL();
        break;
        //Move Left
    case Qt::Key_A:
        eye[0]-=0.1;
        lookAt[0]-=0.1;
        updateGL();
        break;
        //Move Right
    case Qt::Key_D:
        eye[0]+=0.1;
        lookAt[0]+=0.1;
        updateGL();
        break;

    default:
        event->ignore();
        break;
    }
}
