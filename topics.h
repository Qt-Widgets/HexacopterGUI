#ifndef TOPICS_H
#define TOPICS_H
#define ACTIVE          217
#include "gateway-linux/api/gateway/topiclistreport.h"
#include "typeinfo"
#include "datatypes.h"
#include "matlib.h"

#define OFFSET                      3000
#define STATE_TID                   (1002 + OFFSET)

#define SET_PID_VALUES_TID          3001
#define TOPICS_TO_FORWARD_TID       3002

#define ENGINE_CTRL_TID             2001
#define DESIRED_YPR_TID             2002
#define DESIRED_THRUST_TID           2003

#define HEARTBEAT_TID               9999
#define OT_TID                      -1
struct Topic{
    long id;
    const char* name;
    const char* labels;
    int data_length;
};

Topic myTopics[] = {{1001 + OFFSET, "IMU raw", "AccX,AccY,AccZ,GyroX,GyroY,GyroZ,MagX,MagY,MagZ", sizeof(Sensor_9DOF_t) / sizeof(double)},
                    {1002 + OFFSET, "System state", "q0,q1,q2,q3,wx,wy,wz,roll,pitch,yaw", sizeof(State_t) / sizeof(double)},      // Special case!
                    {OT_TID,            "Optical tracking", "roll_ot,pitch_ot,yaw_ot", sizeof(RODOS::Vector3D) / sizeof(double)},       // Special case!
                    {1003 + OFFSET, "PID roll", "P_r,I_r,D_r", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {1004 + OFFSET, "PID pitch", "P_p,I_p,D_p", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {1005 + OFFSET, "PID yaw", "P_y,I_y,D_y", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {1006 + OFFSET, "Control torque", "roll_ctrl,pitch_ctrl,yaw_ctrl", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {1007 + OFFSET, "Motor cmd", "r1,r2,r3,r4,r5,r6", sizeof(motor_cmd_t) / sizeof(double)},
                    {2001 + OFFSET, "Engine", "Set", sizeof(double) / sizeof(double)},
                    {2002 + OFFSET, "Desired YPR", "yaw_set,pitch_set,roll_set", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {2003 + OFFSET, "Desired Thrust", "thrust", sizeof(double) / sizeof(double)}
                   };

#endif // TOPICS_H
