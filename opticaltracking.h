#ifndef OPTICALTRACKING_H
#define OPTICALTRACKING_H

#include "QTimer"
#include <QThread>

#include "vrpn_Tracker.h"
#include "quat.h"



class OpticalTracking : public QThread
{
    Q_OBJECT

signals:
    void ot_attitude(double roll, double pitch, double yaw);
//    void ot_quat(double q0, double q1, double q2, double q3);
public:
    explicit OpticalTracking(QObject *parent);
    void run();
};

#endif // OPTICALTRACKING_H
