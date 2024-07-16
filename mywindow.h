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
#include <QRandomGenerator>
#include <cmath>
class LineChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineChartWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

    // 设置数据点的方法
    void setdesData() {
//        dataPoints = data;
//        我们直接随机生成data  然后处理后得到srcdata
        srcdata.clear();
        //!设置精度



        QRandomGenerator randomgen;
        quint32 seed = QRandomGenerator::global()->bounded(0,100);
        randomgen.seed(seed);
        for (int i = 0; i < 100; i+=10) {
            // 这里应填入你的实际数据点
            qreal y = (static_cast<qreal>(randomgen.bounded(500)));
            qreal x = (static_cast<qreal>(randomgen.bounded(500)));
            min_Y = y>min_Y?min_Y:y;
            max_Y = y>max_Y?y:max_Y;
            srcdata.append(QPointF(x, y)); // someFunction 应替换为你的数据生成逻辑
        }

//        QPainter painter(this);
//         绘制数据点
//        QBrush brush(Qt::blue);
//        painter.setBrush(brush);
//        for (const QPointF &point : srcdata) {
//            painter.drawEllipse(point, 10, 10);
//        }

        //!     给数据点排序
        std::sort(srcdata.begin(),srcdata.end(),[](const QPointF &a,const QPointF &b)
        {
           return a.x()== b.x()?a.y() < b.y():a.x() < b.x();
        });
        drawPoint(srcdata);

        desdata1.clear();
        desdata2.clear();
        getDesdata1();
        getDeadata2();

//        update(); // 通知QWidget重绘   也就是这里就给了信号了？？？
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
        QBrush brush(Qt::black);
        painter.setBrush(brush);
        for (const QPointF &point : srcdata) {
            qreal index_X = point.x();
            qreal index_Y = point.y();
            min_Y = qMin(index_Y,min_Y);
            max_Y = qMax(index_Y,max_Y);
            painter.drawEllipse(point, 50, 50);
            qDebug()<<"point darwed";
        }
    }
    void getDesdata1(){

        qreal precis = 1e-2;

        for(qreal t = 0;t < 1.0000; t+=precis){
            int size = srcdata.size();
            QVector<qreal> coefficient(size, 0);
            coefficient[0] = 1.000;
            qreal u1 = 1.0 - t;
            //!这里就是递归的计算
            //! 如果我们利用
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
            desdata1.append(resultPoint);
        }
   }
    void getDeadata2(){
        //! 牛顿迭代法(Newton)
        //! 牛顿法得到的是位置而不是时间  所以不好计算Func(x_k),grad_Func(x_k)
        //! 所以牛顿法不适用于计算贝塞尔曲线
        //!
        //!
        //!
        //! 矩阵方法
        int size = srcdata.size();
        QVector<int> pascaTri(size+1,0);
        pascaTri[1] = 1;
        for(int i = 1;i<=size;++i){
            int temp1 = pascaTri[0];
            int temp2 = 0;
            for(int j = 1;j<=i;++j){
                temp2 = pascaTri[j];
                pascaTri[j]=temp1+temp2;
                temp1 = temp2;
            }
        }
        qreal precis = 1e-2;
        for(qreal t = 0;t<1.0000;t+=precis){
            QPointF resultPoint={0,0};
            QVector<qreal> bernstein1(size,1),bernstein2(size,1);

            for(int i = 1;i<=size-1;++i){
                bernstein1[i]*=bernstein1[i-1]*t;
                bernstein2[size-i-1]*=bernstein2[size-i]*(1-t);
            }
            for(int i = 0;i<size;++i){
                resultPoint+=srcdata[i]*pascaTri[i+1]*bernstein1[i]*bernstein2[i];
            }
            desdata2.append(resultPoint);
        }

    }
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);

        // 绘制折线图
//        setdesdata();
        drawLineChart1(&painter);
        drawLineChart2(&painter);

    }
    void resizeEvent(QResizeEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        updateChart(&painter);
//        QWidget::resizeEvent(event);
//        setdesData();
        //!  没有这句话依然会调用paintevent
        update();
    }

private:
    void updateChart(QPainter *painter){
//        QSize windowsize = this->size();
        qDebug()<<this->size()<<"minmax"<<min_Y<< "\t"<<max_Y;
//        qreal wid = windowsize.width();
//        qreal hei = windowsize.height();
        QList<QPointF>::reverse_iterator it = desdata1.rbegin();
        qreal max_X = (*it).x();
        for(QPointF &point:desdata1){
            point.setX(point.x()*this->size().width()/max_X);
//            point.setY((point.y()-min_Y)*hei/(max_Y-min_Y));
            //!因为这里我们没有实时更新srcdata的min_Y、max_Y
            //! 所以这里的min_Y、max_Y没有实时更新  因此放缩一下就会瞬间爆炸
            //! 所以必须得把随机生成的数据用srcdata保存起来   并且在resizeevent
            //! 发送update（）以后我们同样的要重新绘制数据点   同时更新新的曲线
            //! 这样我们可以实时的更新min_Y、max_Y  从而达到自适应界面的问题
        }
        drawLineChart1(painter);
    }
    void drawLineChart1(QPainter *painter) {
        if (desdata1.isEmpty()) return;

        QPen pen(Qt::blue);
        pen.setWidth(1);
        painter->setPen(pen);
//        painter->translate(0,50);
        QList<QPointF>::reverse_iterator it = desdata1.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata1.size() - 1; ++i) {
            QPointF start = desdata1[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata1[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
        }


    }
    void drawLineChart2(QPainter *painter) {
        if (desdata2.isEmpty()) return;

        QPen pen(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->translate(0,50);
        QList<QPointF>::reverse_iterator it = desdata2.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata2.size() - 1; ++i) {
            QPointF start = desdata2[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata2[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
        }


    }
    QList<QPointF> srcdata;
    QList<QPointF> desdata1,desdata2; // 存储数据点
    qreal min_Y,max_Y;
};

#endif // MYWINDOW_H
