#ifndef TOPICS_H
#define TOPICS_H
#define ACTIVE          217
#include "gateway-linux/api/gateway/topiclistreport.h"
#include "typeinfo"
#include "datatypes.h"
#include "matlib.h"

#define OFFSET                      3000
#define ATTITUDE_TID                (1031 + OFFSET)
#define STATE_TID                   (1033 + OFFSET)
#define GPS_TID                     (1006 + OFFSET)
#define BAT_TID                     (1010 + OFFSET)

#define SET_PID_VALUES_TID          3001
#define TOPICS_TO_FORWARD_TID       3004
#define SET_PID_ALT_VALUES_TID      3005
#define FLIGHT_CTRL_TID             3006

#define ENGINE_CTRL_TID             2001
#define DESIRED_YPR_TID             2002
#define DESIRED_THRUST_TID           2003

//#define HEARTBEAT_TID               9999
#define OT_TID                      -1
struct Topic{
    long id;
    const char* name;
    const char* labels;
    int data_length;
};

Topic myTopics[] = {
                    {1031 + OFFSET, "Attitude", "q0,q1,q2,q3,wx,wy,wz,roll,pitch,yaw", sizeof(Attitude_t) / sizeof(double)},      // Special case!
                    {1032 + OFFSET, "Altitude", "r_height,r_speed,a_height,a_speed", sizeof(Altitude_t) / sizeof(double)},

                    {1001 + OFFSET, "LSM9DS0", "AccX,AccY,AccZ,GyroX,GyroY,GyroZ,MagX,MagY,MagZ", sizeof(Sensor_9DOF_t) / sizeof(double)},
                    {1002 + OFFSET, "MS5611", "Pressure,Temperatue,Height", sizeof(Sensor_Pressure_t) / sizeof(double)},
                    {1005 + OFFSET, "Lidar", "Height_LiDar,Speed_LiDar,Raw", sizeof(Sensor_Lidar_t) / sizeof(double)},
                    {1009 + OFFSET, "SRF02", "Height_SRF02", sizeof(Sensor_Proximity_t) / sizeof(double)},
                    {1010 + OFFSET, "Battery", "Voltage,Percent", sizeof(Sensor_Voltage_t) / sizeof(double)},                             // Special case!
//                    {1006 + OFFSET, "GPS Data", "iTow, lat, lon, height, hMSL, NEDx, NEDy, NEDz, velNEDx, velNEDy, velNEDz, speed, gSpeed, heading, pAcc, sAcc, cAcc", sizeof(Gps_data_t) / sizeof(double)},
                    {2001 + OFFSET, "Engine", "ARMED", sizeof(double) / sizeof(double)},

                    {2002 + OFFSET, "Remote YPR", "yaw_set,pitch_set,roll_set", sizeof(RODOS::Vector3D) / sizeof(double)},
                    {1041 + OFFSET, "Torque", "Roll_ctrl,Pitch_ctrl,Yaw_ctrl", sizeof(RODOS::Vector3D) / sizeof(double)},

                    {2003 + OFFSET, "Desired Height", "height_set", sizeof(double) / sizeof(double)},
                    {2004 + OFFSET, "Remote Thrust", "thrust_set", sizeof(double) / sizeof(double)},
                    {1042 + OFFSET, "Thrust", "thrust", sizeof(double) / sizeof(double)},

                    {1043 + OFFSET, "Motor CMD", "r1,r2,r3,r4,r5,r6", sizeof(Motor_cmd_t) / sizeof(double)},





                    {OT_TID,        "Optical tracking", "roll_ot,pitch_ot,yaw_ot", sizeof(RODOS::Vector3D) / sizeof(double)}       // Special case!
                   };

#endif // TOPICS_H
