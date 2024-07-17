#include "mywindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>

#include <QRandomGenerator>

#include <QDebug>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow mainWindow;


    LineChartWidget *chartWidget = new LineChartWidget(&mainWindow);

    QWidget *centralWidget = new QWidget(&mainWindow);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(chartWidget);

    qDebug()<<chartWidget->size();



    chartWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    chartWidget->setdesData();


    mainWindow.setCentralWidget(chartWidget);


    mainWindow.show();
    return app.exec();
}
