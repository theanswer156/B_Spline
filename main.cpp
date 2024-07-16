#include "mywindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>
#include <QVector>
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QTime>
#include <QDebug>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow mainWindow;

//    QWidget *centralWidget = new QWidget(&mainWindow);
//    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建折线图小部件
    LineChartWidget *chartWidget = new LineChartWidget(&mainWindow);

    QWidget *centralWidget = new QWidget(&mainWindow);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(chartWidget);
    // 假设这是你已有的数据
//    QList<QPointF> myData;

//    QRandomGenerator randomgen;
//    quint64 seed = QDateTime::currentMSecsSinceEpoch();
//    randomgen.seed(seed);
    qDebug()<<chartWidget->size();
//    QRandomGenerator randomgen;
//    quint32 seed = QRandomGenerator::global()->bounded(0,100);
//    randomgen.seed(seed);
//    for (int i = 0; i < 250; i+=10) {
//        // 这里应填入你的实际数据点

//        myData.append(QPointF(i, (static_cast<float>(randomgen.bounded(500))))); // someFunction 应替换为你的数据生成逻辑
//    }


    chartWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    // 设置数据到折线图小部件
//    chartWidget->drawPoint();
    chartWidget->setdesData();

//    layout->addWidget(chartWidget);
    mainWindow.setCentralWidget(chartWidget);
//    mainWindow.setLayout(layout);

    mainWindow.show();
    return app.exec();
}
