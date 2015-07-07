#ifndef COPTER2D_H
#define COPTER2D_H

#include <QObject>
#include <qcustomplot/qcustomplot.h>

class Copter2D : public QCPCurve
{
    Q_OBJECT
    double arm;
    QCPCurve path;
    QCPCurve green;
    QCPCurve orange;
    double heading, x, y;

    bool rescale;

    void trackPath();
public:
    explicit Copter2D(QCPAxis *keyAxis, QCPAxis *valueAxis, double heading = 0, QObject *parent = 0);

    void drawArms();
    void updatePose(double x, double y, double rot);
    void setHeading(double angle);
    void setPosition(double x, double y);
    void reset();

public slots:
    void scale(QCPRange range);
};

#endif // COPTER2D_H
