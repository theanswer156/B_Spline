#ifndef MYWINDOW_H
#define MYWINDOW_H
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QVector>
#include <QRect>

class MyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MyWindow(QWidget *parent = nullptr):QWidget(parent)
    {
        initData();
    }
    void initData()
    {
        // 随机生成一些数据点
//        Qt默认界面的坐标原点在左上角
        for (int i = 0; i < 1000; i+=20) {
            dataPoints.append(QPointF(i, static_cast<float>(rand()) / RAND_MAX * 100));
        }
    }
//    ~MyWindow()override;
protected:
    void paintEvent(QPaintEvent *event) override{
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        drawLineChart(&painter);
    }

private:
    void drawLineChart(QPainter *painter){
        QPen pen(Qt::red);
        pen.setWidth(1);
        painter->translate(0,50);
        painter->setPen(pen);
//        绘制折线图
        for (int i = 0; i < dataPoints.size() - 1; i++) {
            QPointF start = dataPoints[i];
            QPointF end = dataPoints[i + 1];
            painter->drawLine(start, end);
        }
//        标记数据点
        QBrush brush(Qt::blue);

        for(const QPointF &point:dataPoints){
            painter->setBrush(brush);
            painter->drawEllipse(point,2,2);
        }
    }
    QVector<QPointF> dataPoints;
};

#endif // MYWINDOW_H
