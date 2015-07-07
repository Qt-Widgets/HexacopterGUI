#include "copter2d.h"
#include "QDebug"


Copter2D::Copter2D(QCPAxis *keyAxis, QCPAxis *valueAxis, double heading, QObject *parent) :
    QCPCurve(keyAxis, valueAxis),
    path(keyAxis, valueAxis),
    green(keyAxis, valueAxis),
    orange(keyAxis, valueAxis)
{
    arm = 2.0;
    heading = 0;

    rescale = false;

    drawArms();
    path.setPen(QPen(QColor::fromRgb(255, 117, 25), 2.0));

    trackPath();
}

void Copter2D::trackPath()
{
    QCPCurveDataMap *dataMap = this->data();
    double x = dataMap->first().key;
    double y = dataMap->first().value;
    path.addData(x,y);
}

void Copter2D::drawArms(){
    this->clearData();
    green.clearData();
    orange.clearData();

    this->addData(0,0);
    for (int i = 90; i > -270; i = i - 120){
        this->addData(arm * cos(i/180.*M_PI), arm * sin(i/180.*M_PI));
        this->addData(0,0);
    }
    this->setPen(QPen(Qt::darkGray, 4.0));

    green.addData(0,0);
    green.addData(arm * cos(30/180.*M_PI), arm * sin(30/180.*M_PI));
    green.addData(0,0);
    green.addData(arm * cos(150/180.*M_PI), arm * sin(150/180.*M_PI));
    green.addData(0,0);
    green.setPen(QPen(QColor::fromRgb(102, 255, 51), 4.0));

    orange.addData(0,0);
    orange.addData(arm * cos(270/180.*M_PI), arm * sin(270/180.*M_PI));
    orange.addData(0,0);
    orange.setPen(QPen(QColor::fromRgb(255, 117, 25), 4.0));
}

void Copter2D::updatePose(double x, double y, double rot){
    drawArms();
    setHeading(rot);
    setPosition(x,y);
}

void Copter2D::setHeading(double heading)
{
    this->heading = heading;
    QCPCurveDataMap *dataMap = this->data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key * cos(-heading) - data.value * sin(-heading);
        double newY = data.key * sin(-heading) + data.value * cos(-heading);
        this->removeData(data.t);
        this->addData(newX, newY);
    }
    dataMap = green.data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key * cos(-heading) - data.value * sin(-heading);
        double newY = data.key * sin(-heading) + data.value * cos(-heading);
        green.removeData(data.t);
        green.addData(newX, newY);
    }
    dataMap = orange.data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key * cos(-heading) - data.value * sin(-heading);
        double newY = data.key * sin(-heading) + data.value * cos(-heading);
        orange.removeData(data.t);
        orange.addData(newX, newY);
    }
}

void Copter2D::setPosition(double x, double y)
{
    this->x = x;
    this->y = y;
    QCPCurveDataMap *dataMap = this->data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key + x;
        double newY = data.value + y;
        this->addData(newX, newY);
        this->removeData(data.t);
    }
    dataMap = green.data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key + x;
        double newY = data.value + y;
        green.addData(newX, newY);
        green.removeData(data.t);
    }
    dataMap = orange.data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key + x;
        double newY = data.value + y;
        orange.addData(newX, newY);
        orange.removeData(data.t);
    }
    trackPath();
}

void Copter2D::reset()
{
    qDebug() << path.data()->size();
    path.clearData();

    qDebug() << path.data()->size();
    trackPath();
}


void Copter2D::scale(QCPRange range)
{
    double dist = range.upper - range.lower;
    arm = dist / 50.;
    updatePose(x,y,heading);


}
