#ifndef MAPPLOT_H
#define MAPPLOT_H

#include <qcustomplot/qcustomplot.h>
#include <Mapplot/copter2d.h>
#include <Mapplot/ship.h>

class Mapplot : public QCustomPlot
{
    Ship *ship;
    Copter2D *copter;
public:
    Mapplot();
    void setPoseDrone(double x, double y, double angle);
    void resetGPS();
};

#endif // MAPPLOT_H
