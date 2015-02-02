#include "opticaltracking.h"
#include "QDebug"
#include "matlib.h"

void VRPN_CALLBACK handle_tracker(void *userData, const vrpn_TRACKERCB t)
{
    RODOS::AngleAxis angleAxis(M_PI/2,1,0,0);
    RODOS::Quaternion rot(angleAxis);
    RODOS::Quaternion quat(t.quat[3],t.quat[0],t.quat[1],t.quat[2]);
    quat = rot * quat;
    RODOS::YPR euler(quat);
    OpticalTracking* ot = (OpticalTracking*) userData;
    emit ot->ot_attitude(euler.roll - M_PI / 2, -euler.pitch, -euler.yaw);
}

OpticalTracking::OpticalTracking(QObject *parent)
    : QThread(parent)
{

}

void OpticalTracking::run()
{
    bool running = true;
    vrpn_Tracker_Remote* vrpn_Tracker;

    vrpn_Tracker = new vrpn_Tracker_Remote("Rigid Body 1@132.187.210.28");
    vrpn_Tracker->register_change_handler(this, handle_tracker );

    while(running)
    {
        vrpn_Tracker->mainloop();
    }
}

