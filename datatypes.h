/*
 * datatypes.h
 *
 *  Created on: 08.10.2014
 *      Author: Michael Strohmeier
 */

#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "matlib.h"


// Sensors
//#########################################################################
typedef struct sensor_9dof_s{
    RODOS::Vector3D accel;
    RODOS::Vector3D gyro;
    RODOS::Vector3D mag;
} Sensor_9DOF_t;

typedef struct sensor_voltage_s{
    double voltage, percent;
} Sensor_Voltage_t;

typedef struct sensor_proximity_s{
    double height;
} Sensor_Proximity_t;

class Sensor_Pressure_t{
public:
    double pressure, temp, height;
    Sensor_Pressure_t(){
        pressure = temp = height = 0;
    }
};

class Sensor_Lidar_t{
public:
    double height, velocity, raw;
    Sensor_Lidar_t(){
        raw = height = velocity = 0;
    };
};

typedef struct sensor_gps_s{
    unsigned int iTow;
    double lat, lon;
    double height, hMSL;
    RODOS::Vector3D NED;
    RODOS::Vector3D velNED;
    double speed;
    double gSpeed;
    double heading;
    double pAcc;
    double sAcc;
    double cAcc;
} Sensor_GPS_t;

// System
//#########################################################################
typedef struct attitude_s{
    RODOS::Quaternion q;
    RODOS::Vector3D w;
} Attitude_t;

typedef struct altitude_s{
    double r_height;
    double r_speed;
    double a_height;
    double a_speed;
} Altitude_t;


// Actuators
//#########################################################################
typedef struct motor_cmd_s{
    double rot1, rot2, rot3, rot4, rot5, rot6;
} Motor_cmd_t;


// Control
//#########################################################################
typedef struct PID_values_s{
    RODOS::Vector3D inner, outer;
    double outerLimit;
    double omegaAlpha, deltaOmegaAlpha;
}PID_values_t;

typedef struct remote_s{
    short aux2;
    short flap;
    short gear;
    double gas;
}Remote_t;

// Flight Controller States
typedef enum Flight_states_e : char{
    MOTOR_OFF = 0, ARMED, TAKE_OFF, ALTITUDE_HOLD, MANUAL_FLIGHT, LANDING
} Flight_states_t;

#endif /* DATATYPES_H_ */
