#ifndef MYWINDOW_H
#define MYWINDOW_H
#include <QWidget>
#include <QPainter>

#include <QList>

#include <QObject>
#include <QEvent>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>
class LineChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineChartWidget(QWidget *parent = nullptr) : QWidget(parent) {

    }

    void setdesData() {


        srcdata.clear();

        QRandomGenerator randomgen;
        quint32 seed = QRandomGenerator::global()->bounded(0,100);
        randomgen.seed(seed);
        for (int i = 0; i < 200; i+=10) {
            qreal y = (static_cast<qreal>(randomgen.bounded(200)));
            qreal x = (static_cast<qreal>(randomgen.bounded(200)));
            min_Y = y>min_Y?min_Y:y;
            max_Y = y>max_Y?y:max_Y;
            srcdata.append(QPointF(x, y));
        }


        //!     给数据点排序
        std::sort(srcdata.begin(),srcdata.end(),[](const QPointF &a,const QPointF &b)
        {
           return a.x()== b.x()?a.y() < b.y():a.x() < b.x();
        });
        drawPoint();
        drawGrid();

        desdata1.clear();
        getDesdata1();
        desdata2.clear();
        getDesdata2();
        getDesdata_Resampling();
        desdata_CR.clear();
        getDesdata_CR();

//        desdata_H.clear();
//        getDesdata_H();

    }
    void drawGrid(){
        QPainter painter(this);
        QPen pen;
        pen.setWidth(5);
        pen.setColor(Qt::black);
        painter.setPen(pen);
//        painter.setPen(Qt::black);
        int width = this->size().width();
        int height = this->size().height();
        for(int i = 0;i<width;i+=10){
            painter.drawLine(QPoint{i,0},QPoint{i,height});
        }
        for(int j = 0;j<height;j+=10){
            painter.drawLine(QPoint{0,j},QPoint{width,j});
        }
        qDebug()<<"Grid drawed";
    }
    void drawPoint(){
        QPainter painter(this);
        QBrush brush(Qt::green);
        painter.setBrush(brush);
        for (const QPointF &point : srcdata) {
            qreal index_X = point.x();
            qreal index_Y = point.y();
            min_Y = qMin(index_Y,min_Y);
            max_Y = qMax(index_Y,max_Y);
            painter.drawEllipse(point, 5, 5);
        }
        qDebug()<<"point drawed";

    }
    void getDesdata1(){


        for(qreal t = 0;t < 1.0000; t+=2*precis){
            int size = srcdata.size();
            QVector<qreal> coefficient(size, 0);
            coefficient[0] = 1.000;
            qreal u1 = 1.0 - t;
            //!
            //! De Casteljau递推算法
            //!
            //!
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
            desdata3.append(resultPoint);
        }
   }
    void getDesdata2(){
        //! 牛顿迭代法(Newton)
        //! 牛顿法得到的是时间而不是位置  所以不好计算Func(x_k),grad_Func(x_k)
        //! 所以牛顿法不适用于计算贝塞尔曲线

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
    void getDesdata_Resampling(){

        QPainter painter(this);
        painter.setPen(Qt::red);
        MSE = 0;
        qreal reciprocalPrecis = 1/precis;
        int i = 2;
        int count = 1;
        QVector<QPointF> richardson(4);
        //!     不对  如果我们在i-1位置插了一个新点  那么desdata中的值就变了
        //!     那么后面的值也就会变  这样的话  用理查森外推法中心误差公式就
        //!     无法计算准确的数值了   所以前两个和后两个不能按照位置来
        //!     必须在计算后就改变   而不是按照坐标来
        for(qreal t = 0;t<1.0000;t+=precis){
            /*计算currPoint和nextPoint中间点的导数值f'(x)   如果大于阈值的话要画不同的线*/
            if(i<=desdata1.size()-3){
            richardson[0] = desdata1.at(i-2);
            richardson[1] = desdata1.at(i-1);
            richardson[2] = desdata1.at(i+1);
            richardson[3] = desdata1.at(i+2);
            QPointF gradF = (-richardson[3]+8*richardson[2]-8*(richardson[1])+(richardson[0]))*reciprocalPrecis/12;
            painter.drawLine(desdata1[i],(desdata1[i]+gradF));
            QPointF ggradF = (richardson[1]+richardson[2]-2*desdata1[i])*reciprocalPrecis*reciprocalPrecis;
            qreal curvate = gradF.x()*ggradF.y()-(gradF.y()*ggradF.x())/qPow(gradF.x()*gradF.x()+gradF.y()*gradF.y(),0.75);
            QPointF insertPoint = desdata1.at(i) + gradF*precis/2 +ggradF*precis*precis/8;
            qreal dis1 = qPow(qPow((insertPoint.x()-desdata1.at(i).x()),2)+qPow((insertPoint.y()-desdata1.at(i).y()),2),0.5);
            qreal dis2 = qPow(qPow((insertPoint.x()-desdata1.at(i+1).x()),2)+qPow((insertPoint.y()-desdata1.at(i+1).y()),2),0.5);
            //!     把距离两点较近的新点给过滤掉  这样同时能够减少出现超出
            //      会出现插入的点超出两点的范围  这样可以用向量的点乘
            //      如果超出两点范围    则点乘的结果一般为正   角度为锐角
            //      而如果角度为钝角  则为负
            //!     左右两点范围的新点插入
            //!     不把相差较远的点过滤掉会出现超出旁边两点太多  图像出现紊乱
            //!     不过我们可以利用插入点和旁边两点的向量点乘来度量插入点的偏离程度
            //!     偏离程度太大的就都过滤掉
            if(dis1/dis2<1+ratio && dis1/dis2>1-ratio)
            {
                if(abs(gradF.x())>gradThreshold.x() || abs(gradF.y())>gradThreshold.y() || curvate>curvate_threshold){
                //!     依次往右边插值   这里就可以用泰勒公式了

                qDebug()<<i<<"\t"<<count<<"\t"<<i+count<<"\t"<<desdata3.size();
                desdata3.insert(i+count,insertPoint);
                //!     将新插入点与原本的点的距离算出来  计算均方误差
                //!     如果是方差的话  应该会更小
                qreal error = qPow(insertPoint.x()-desdata2.at(i+count-1).x(),2)+qPow(insertPoint.y()-desdata2.at(i+count-1).y(),2);
                MSE+=error;
                qDebug()<<insertPoint<<"\t"<<desdata2.at(i+count-1)<<"\t"<<error;
                count+=1;
                }
            }
            ++i;
            }
        }
        qDebug()<<MSE/count;
    }
    void getDesdata_CR(){
        //!     一个点的速度与相邻两点形成的向量是平行的
        //!     统一用一个量  τ   来形容他们之间的比例关系
        desdata_CR.clear();
        qreal tau = 9e-1;   //!     tau越小曲线越平缓
        for(int i = 2;i<srcdata.size()-1;i+=1){
            for(qreal t = 0;t<1.0000;t+=precis){
                QPointF C_0 = srcdata.at(i-1);
                QPointF C_1 = -tau*srcdata.at(i-2)+tau*srcdata.at(i);
                QPointF C_2 = 2*tau*srcdata.at(i-2)+(tau-3)*srcdata.at(i-1)+(3-2*tau)*srcdata.at(i)-tau*srcdata.at(i+1);
                QPointF C_3 = -tau*srcdata.at(i-2)+(2-tau)*srcdata.at(i-1)+(tau-2)*srcdata.at(i)+tau*srcdata.at(i+1);
                QPointF resultPoint = C_0+C_1*t+C_2*t*t+C_3*t*t*t;
//                qreal B0 = 0.5*qPow((2-t),3);
//                qreal B1 = 0.75*qPow(t,2)-qPow(t,3);
//                qreal B2 = 0.75*(t-1)*qPow(t,2);
//                qreal B3 = 0.5*qPow(t,3);
//                QPointF resultPoint = B0*srcdata.at(i-1)+B1*srcdata.at(i-2)+B2*srcdata.at(i)+B3*srcdata.at(i+1);
                //!     这样每个控制点会画加入两次
                desdata_CR.append(resultPoint);
            }
        }
    }
    void getDesdata_H(){
        desdata_H.clear();
        for(int i = 2;i<srcdata.size()-3;++i){

                //!     理查森外推计算两个点的导数值   4阶中心差分公式
                //!     Hermite曲线：由两个位矢和两个导矢定义
                //!     是由四个点控制的三次插值曲线

                QPointF R0 = (-srcdata.at(i+2)+8*srcdata.at(i+1)-8*srcdata.at(i-1)+srcdata.at(i-2))/(12*precis);
                QPointF R1 = (-srcdata.at(i+3)+8*srcdata.at(i+2)-8*srcdata.at(i)+srcdata.at(i-1))/(12*precis);
                //!     Hermite曲线系数值
                //!     Hermite插值算出来的曲线太抽象了
            for(qreal t = 0;t<1.0000;t+=precis){
                qreal F0 = t*t*(2*t-3)+1;
                qreal F1 = t*t*(3-2*t);
                qreal F2 = t*qPow((t-1),2);
                qreal F3 = t*t*(t-1);
                QPointF resultPoint = srcdata.at(i)*F0+srcdata.at(i+1)*F1+R0*F2+R1*F3;
                desdata_H.append(resultPoint);
            }
        }
        qDebug()<<"Hermite data get";
    }
    void diff(){
        int size = desdata1.size();
        for(int i = 0;i<size;++i){
            if(desdata1.at(i)==desdata3.at(i))
                qDebug()<<"第"<<i<<"项相同";
        }
    }

protected:
    void wheelEvent(QWheelEvent *event)override{
        double scaleFactor = 1.01;

        if(event->angleDelta().y()>0){
            zoomlevel*=scaleFactor;
        }else{
            zoomlevel/=scaleFactor;
        }
        //!     这里就是告诉event参数有改变
        //!     我希望能够根据更改后的参数重绘图像
        event->accept();
        update();
    }
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 记录鼠标按下时的位置
            lastMousePosition = event->pos();
            isDragging = true;
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (isDragging && (event->buttons() & Qt::LeftButton)) {
            // 计算鼠标移动的差值
            QPoint delta = event->pos() - lastMousePosition;
            // 更新图像位置（这里以移动widget为例）
            this->move(x() + delta.x(), y() + delta.y());
            // 更新最后鼠标位置
            lastMousePosition = event->pos();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            isDragging = false;
        }
    }
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);

        //! 保存当前的转换矩阵
        QTransform oldTransform = painter.transform();

        //! 移动到缩放中心的负向位置，然后应用缩放
        painter.translate(-zoomCenter.x(), -zoomCenter.y());
        painter.scale(zoomlevel, zoomlevel);



        painter.scale(zoomlevel,zoomlevel);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);

        // 绘制折线图
//        setdesdata();
//        drawPoint();
//        drawGrid();
        drawLineChart1(&painter);
        drawLineChart3(&painter);
        drawLineChart2(&painter);
        drawLineChart_CR(&painter);

//        drawLineChart_H(&painter);

        //! 恢复之前的转换矩阵
        painter.setTransform(oldTransform);
    }
    void resizeEvent(QResizeEvent *event) override {
        Q_UNUSED(event);

        updateChart();

        //!  没有这句话依然会调用paintevent
//        update();
    }

private:
    void updateChart(){

        qDebug()<<this->size()<<"minmax"<<min_Y<< "\t"<<max_Y;

        min_X = desdata1.at(0).x();
//        painter->translate(min_X,0);
        QList<QPointF>::reverse_iterator it = desdata1.rbegin();
        qreal max_X = (*it).x();
        for(QPointF &point:srcdata)
            point.setX(point.x()*this->size().width()/max_X);
        for(QPointF &point:desdata1)
            point.setX(point.x()*this->size().width()/max_X);
        for(QPointF &point:desdata2)
            point.setX(point.x()*this->size().width()/max_X);
        for(QPointF &point:desdata3)
            point.setX(point.x()*this->size().width()/max_X);
        for(QPointF &point:desdata_CR)
            point.setX(point.x()*this->size().width()/max_X);
        for(QPointF &point:desdata_H)
            point.setX(point.x()*this->size().width()/max_X);
//            point.setY((point.y()+min_Y)*this->size().width()/(max_Y-min_Y));
            //!因为这里我们没有实时更新srcdata的min_Y、max_Y
            //! 所以这里的min_Y、max_Y没有实时更新  因此放缩一下就会瞬间爆炸
            //! 所以必须得把随机生成的数据用srcdata保存起来   并且在resizeevent
            //! 发送update（）以后我们同样的要重新绘制数据点   同时更新新的曲线
            //! 这样我们可以实时的更新min_Y、max_Y  从而达到自适应界面的问题
//        drawLineChart1(painter);
//        drawLineChart2(painter);
//        drawLineChart3(painter);
    }
    void drawLineChart1(QPainter *painter) {
        if (desdata1.isEmpty()) return;

        QPen pen(Qt::blue);
        pen.setWidth(1);
        painter->setPen(pen);
//        painter->translate(0,50);
        QList<QPointF>::reverse_iterator it = desdata1.rbegin();
        qreal max_X = (*it).x();
        painter->setBrush(Qt::blue);
        for(const QPointF &point:srcdata){

            painter->drawEllipse(point,Point_Radius,Point_Radius);
        }
        painter->setBrush(Qt::NoBrush);
        for (int i = 0; i < desdata1.size() - 1; ++i) {
            QPointF start = desdata1[i];
            start.setX((start.x())*this->size().width()/max_X);
            QPointF end = desdata1[i + 1];
            end.setX((end.x())*this->size().width()/max_X);
            painter->drawLine(start, end);
            painter->drawEllipse(start,Point_Radius,Point_Radius);
        }
    }
    void drawLineChart2(QPainter *painter) {
        if (desdata2.isEmpty()) return;

        QPen pen(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->translate(0,20);
        QList<QPointF>::reverse_iterator it = desdata2.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata2.size() - 1; ++i) {
            QPointF start = desdata2[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata2[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
            painter->drawEllipse(start,Point_Radius,Point_Radius);
        }
    }
    void drawLineChart3(QPainter *painter) {
        if (desdata3.isEmpty()) return;

        std::sort(desdata3.begin(),desdata3.end(),[](const QPointF &a,const QPointF &b){
            return a.x()==b.x()?a.y()<b.y():a.x()<b.x();
        });

        QPen pen(Qt::black);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->translate(0,20);
        QList<QPointF>::reverse_iterator it = desdata2.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata3.size() - 1; ++i) {
            QPointF start = desdata3[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata3[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
            /*painter->setBrush(Qt::black);*/
            painter->drawEllipse(start,Point_Radius,Point_Radius);
        }
    }
    void drawLineChart_CR(QPainter *painter) {
        if (desdata_CR.isEmpty()) return;

        QPen pen(Qt::black);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->translate(0,20);
        QList<QPointF>::reverse_iterator it = desdata_CR.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata_CR.size() - 1; ++i) {
            QPointF start = desdata_CR[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata_CR[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
            painter->drawEllipse(start,Point_Radius,Point_Radius);
        }
        qDebug()<<"Line CR drawed";
    }
    void drawLineChart_H(QPainter *painter) {
        if (desdata_CR.isEmpty()) return;

        QPen pen(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->translate(0,20);
        QList<QPointF>::reverse_iterator it = desdata_CR.rbegin();
        qreal max_X = (*it).x();
        for (int i = 0; i < desdata_H.size() - 1; ++i) {
            QPointF start = desdata_H[i];
            start.setX(start.x()*this->size().width()/max_X);
            QPointF end = desdata_H[i + 1];
            end.setX(end.x()*this->size().width()/max_X);
            painter->drawLine(start, end);
            painter->drawEllipse(start,Point_Radius,Point_Radius);
        }
        qDebug()<<"Line Hermite drawed";
    }

    QList<QPointF> srcdata;//   存储原始数据点
    QList<QPointF> desdata1,desdata2,desdata3,desdata_CR,desdata_H; // 存储数据点
    qreal min_Y,max_Y;
    qreal min_X;
    QPointF zoomCenter;
    QPointF gradThreshold={2e2,2e2};
    double zoomlevel = 1.0;
    //!     记录鼠标按下时的位置
    QPoint lastMousePosition;
    //!     标记是否在拖拽
    bool isDragging = false;
    qreal ratio = 7e-1;
    qreal precis = 5e-3;
    qreal curvate_threshold = 1e2;
    qreal MSE;
    int Point_Radius = 3;
};

#endif
// MYWINDOW_H
