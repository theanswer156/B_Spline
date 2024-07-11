#ifndef MYWINDOW_H
#define MYWINDOW_H
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QList>
#include <QRect>
#include <QObject>
#include <QEvent>
#include <QDebug>

class MyEventFilter : public QWidget
{
    Q_OBJECT

protected:
     bool eventFilter(QObject *obj,QEvent *event) override
    {
         if(event->type() == QEvent::MouseButtonPress){
             getData(dataPoints);
             QPainter paint(this);
             drawLineChart(&paint);
         }
         return QObject::eventFilter(obj,event);
    }
    void paintEvent(QPaintEvent *event)override
    {
        Q_UNUSED(event);
        QPainter paint(this);
        paint.setRenderHint(QPainter::Antialiasing);
        drawLineChart(&paint);

    }


     void getData(QList<QPointF> &desdata){
         //     更新画图数据   按给定的顺序排列
         //!设置精度
         qreal precis = 0.01;

         QList<QPointF> src;
         // 随机生成一些数据点
         for (int i = 0; i < 10; ++i) {
             src.append(QPointF(i, static_cast<float>(rand()) / RAND_MAX * 100));
         }
         for(qreal t = 0;t < 1.0000; t+=precis){
             int size = src.size();
             QVector<qreal> coefficient(size, 0);
             coefficient[0] = 1.000;
             qreal u1 = 1.0 - t;
             for (int j = 1; j <= size - 1; j++) {
                 qreal saved = 0.0;
                 for (int k = 0; k < j; k++){
                     qreal temp = coefficient[k];
                     coefficient[k] = saved + u1 * temp;
                     saved = t * temp;
                 }
                 coefficient[j] = saved;
             }

             QPointF resultPoint;
             for (int i = 0; i < size; i++) {
                 QPointF point = src.at(i);
                 resultPoint = resultPoint + point * coefficient[i];
             }
             desdata.append(resultPoint);
         }
     }
private:
     void drawLineChart(QPainter *paint){
         QPen pen;
         pen.setColor(Qt::yellow);
         pen.setWidth(1);
         QBrush brush(Qt::blue);
         paint->setBrush(brush);
         paint->setPen(pen);

         for(QList<QPointF>::iterator it = dataPoints.begin();it!=dataPoints.end();){
             paint->drawLine(*it,*(++it));
             paint->drawEllipse(*it,2,2);
         }
     }
private:
    QList<QPointF> dataPoints = {
        {1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}
    };
};

class LineChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineChartWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

    // 设置数据点的方法
    void setdesData(QList<QPointF> &srcdata) {
//        dataPoints = data;
//        我们直接随机生成data  然后处理后得到srcdata

        //!设置精度
        qreal precis = 0.001;


        // 随机生成一些数据点
//        for (int i = 0; i < 100; i+=1) {
//            srcdata.append(QPointF(i, static_cast<float>(rand()) / RAND_MAX * 100));
//        }

        QPainter painter(this);
        //         绘制数据点
        QBrush brush(Qt::blue);
        painter.setBrush(brush);
        for (const QPointF &point : srcdata) {
            painter.drawEllipse(point, 2, 2);
        }




        for(qreal t = 0;t < 1.0000; t+=precis){
            int size = srcdata.size();
            QVector<qreal> coefficient(size, 0);
            coefficient[0] = 1.000;
            qreal u1 = 1.0 - t;
            for (int j = 1; j <= size - 1; j++) {
                qreal saved = 0.0;
                for (int k = 0; k < j; k++){
                    qreal temp = coefficient[k];
                    coefficient[k] = saved + u1 * temp;
                    saved = t * temp;
                }
                coefficient[j] = saved;
            }

            QPointF resultPoint;
            for (int i = 0; i < size; i++) {
                QPointF point = srcdata.at(i);
                resultPoint = resultPoint + point * coefficient[i];
            }
            desdata.append(resultPoint);
        }


        update(); // 通知QWidget重绘   也就是这里就给了信号了？？？
        //!updata（）就会通知这个类LineChartWidget要求重绘
        //! 这个时候就会调用paintevent事件处理器函数  而我们在子类中重写了
        //! paintevent函数   所以就会调用重写的paintevent事件处理器函数
        //! 从而完成我们想要的事件
        //!
        //! 所以我们只需要建立信号槽机制  使得收到信号时setdata就行了？？？
    }
    void drawPoint(QList<QPointF> srcdata){
        QPainter painter(this);
//         绘制数据点
        QBrush brush(Qt::blue);
        painter.setBrush(brush);
        for (const QPointF &point : srcdata) {
            qreal index_Y = point.y();
            min_Y = qMin(index_Y,min_Y);
            max_Y = qMax(index_Y,max_Y);
            painter.drawEllipse(point, 3, 3);
        }
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制折线图
        drawLineChart(&painter);
    }
    void resizeEvent(QResizeEvent *event) override {
        QPainter painter(this);
        updateChart(&painter);
        QWidget::resizeEvent(event);
    }

private:
    void updateChart(QPainter *painter){
        QSize windowsize = this->size();
        qDebug()<<windowsize<<"minmax"<<min_Y<<max_Y;
        qreal wid = windowsize.width();
//        qreal hei = windowsize.height();
        QList<QPointF>::reverse_iterator it = desdata.rbegin();
        qreal max_X = (*it).x();
        for(QPointF &point:desdata){
            point.setX(point.x()*wid/max_X);
//            point.setY((point.y()-min_Y)*hei/(max_Y-min_Y));
        }
        drawLineChart(painter);
    }
    void drawLineChart(QPainter *painter) {
        if (desdata.isEmpty()) return;

        QPen pen(Qt::blue);
        pen.setWidth(1);
        painter->setPen(pen);

        for (int i = 0; i < desdata.size() - 1; ++i) {
            QPointF start = desdata[i];
            QPointF end = desdata[i + 1];
            painter->drawLine(start, end);
        }


    }


    QList<QPointF> desdata; // 存储数据点
    qreal min_Y,max_Y;
};

#endif // MYWINDOW_H
