#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#define WINDOW_WIDTH  450
#define WINDOW_HEIGHT 450

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:

public slots:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);

private:
    QSize windowSize;

};

#endif // GLWIDGET_H
