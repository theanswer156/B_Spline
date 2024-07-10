#include "mywindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    //这里没有指定父类  后面用的时候要以引用的形式使用
    QWidget *centralWidget = new QWidget(&w);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    MyWindow *mywindow = new MyWindow();
    mywindow->setFixedSize(1000,200);
    layout->addWidget(mywindow);
    w.setCentralWidget(centralWidget);
    w.show();

    return a.exec();
}
