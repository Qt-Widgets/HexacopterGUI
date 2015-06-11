#ifndef MAPPLOT_H
#define MAPPLOT_H

#include <qcustomplot/qcustomplot.h>
#include <Mapplot/copter2d.h>
#include <Mapplot/ship.h>

class Mapplot : public QCustomPlot
{
    Ship *ship;
    Copter2D *copter;
    double copterX, copterY;
public:
    Mapplot();
    void moveDrone(double x, double y);
};

#endif // MAPPLOT_H
