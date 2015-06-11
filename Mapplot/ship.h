#ifndef SHIP_H
#define SHIP_H

#include "qcustomplot/qcustomplot.h"

class Ship : public QCPCurve {
    static const double x[12];
    static const double y[12];
    QCPCurve path;
    void trackPath();
public:
    Ship(QCPAxis *keyAxis, QCPAxis *valueAxis, double heading = 0);
    void rotate(double angle);
    void translate(double x, double y);
};

#endif // SHIP_H
