#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    GLWidget *glWidget = new GLWidget(this);
    setCentralWidget(glWidget);
    //Pass all input directly to the GLWidget
    glWidget->setFocus(Qt::ActiveWindowFocusReason);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::exitProgram()
{

}

