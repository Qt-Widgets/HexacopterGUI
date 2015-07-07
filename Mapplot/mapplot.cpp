#include "Mapplot/mapplot.h"

Mapplot::Mapplot() : QCustomPlot()
{
    this->axisRect()->setBackground(QColor::fromRgb(0, 153, 255));
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectAxes | QCP::iSelectPlottables);
    this->xAxis->setRange(-100,100);
    this->yAxis->setScaleRatio(this->xAxis, (double) this->size().width() / (double)   this->size().height() );


    ship = new Ship(this->xAxis, this->yAxis);
    this->addPlottable(ship);
    this->replot();

    copter = new Copter2D(this->xAxis, this->yAxis);
    this->addPlottable(copter);
    this->replot();

    qRegisterMetaType<QCPRange>("QCPRange");
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), copter, SLOT(scale(QCPRange)),Qt::QueuedConnection);
}

void Mapplot::setPoseDrone(double x, double y, double angle)
{
    copter->updatePose(x,y,angle);
    this->replot();
}

void Mapplot::resetGPS()
{
    copter->reset();
    this->replot();
}


