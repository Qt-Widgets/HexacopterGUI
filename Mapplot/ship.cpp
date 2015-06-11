#include "ship.h"
#include "QDebug"
const double Ship::x[12] = {-12.5, -12.5, -6, -4, 0, 4, 6, 12.5, 12.5, 11, -11, -12.5};
//const double Ship::y[12] = {-17.7, 47.2, 75, 80, 82.6, 80, 75, 47.2, -17.7, -35.4, - 35.4, -17.7}; // Heli port
const double Ship::y[12] = {-41.3, 23.6, 51.4, 56.4, 59, 56.4, 51.4, 23.6, -41.3, -59, -59, -41.3}; // Center

Ship::Ship(QCPAxis *keyAxis, QCPAxis *valueAxis, double heading) :
    QCPCurve(keyAxis, valueAxis),
    path(keyAxis, valueAxis)
{
    for (int i = 0; i < 12; i++)
        this->addData(x[i] * cos(heading) - y[i] * sin(heading), x[i] * sin(heading) + y[i] * cos(heading));
    this->setPen(QPen(Qt::darkBlue, 3));
    this->setBrush(Qt::white);
    trackPath();
}

void Ship::trackPath()
{
    double sumX = 0, sumY = 0;
    QCPCurveDataMap *dataMap = this->data();
    foreach(QCPCurveData data, *dataMap){
        sumX += data.key;
        sumY += data.value;
    }
    sumX -= dataMap->last().key;
    sumY -= dataMap->last().value;
    path.addData(sumX/(dataMap->size() - 1),sumY/(dataMap->size() - 1));
    path.setPen(QPen(Qt::darkBlue));
    path.setLineStyle(QCPCurve::lsLine);
    path.setVisible(true);
}

void Ship::rotate(double angle)
{
    QCPCurveDataMap *dataMap = this->data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key * cos(angle) - data.value * sin(angle);
        double newY = data.key * sin(angle) + data.value * cos(angle);
        this->removeData(data.t);
        this->addData(newX, newY);
    }
}

void Ship::translate(double x, double y)
{
    QCPCurveDataMap *dataMap = this->data();
    foreach(QCPCurveData data, *dataMap){
        double newX = data.key + x;
        double newY = data.value + y;
        this->removeData(data.t);
        this->addData(newX, newY);
    }
    trackPath();
}
