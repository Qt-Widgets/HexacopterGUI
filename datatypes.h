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

////////////////////////////////////////////////////////////////////////////
// Short topics for low data rates

typedef struct{
    float x,y,z;
} Vector3Df_t;

typedef struct{
    float q0;
    Vector3Df_t q;
} Quaternionf_t;

typedef struct{
    float height, speed;
} Proximityf_t;

typedef struct{
    float height;
} Height_t;

typedef struct{
    float percent;
} Batteryf_t;

typedef struct {
    float lat, lon;
    float height;
    Vector3Df_t NED;
} GPS_small_t;

typedef struct {
    Flight_states_t state;
    Quaternionf_t quat;
    GPS_small_t gps;
    Batteryf_t battery;
    Proximityf_t lidar;
    Height_t baro;
    Height_t sonic;
} System_State_t;

class Vector2D{
public:
    double x,y;
    Vector2D(double x, double y){
        this->x = x;
        this->y = y;
    }
    Vector2D operator+(const Vector2D &r){
        return Vector2D(this->x + r.x, this->y + r.y);
    }
    Vector2D operator-(const Vector2D &r){
        return Vector2D(this->x - r.x, this->y - r.y);
    }
    Vector2D rotate(double angle){
        double ca = cos(angle);
        double sa = sin(angle);
        return Vector2D( x* ca - y * sa, x * sa + y * ca);
    }
};

typedef enum frame : char {
    BODY = 0, NED, GEO
} frame_t;


class Pose_t{
public:
    Vector2D v;
    double phi;
    frame_t frame;
    Pose_t():
        v(0,0)
    {
        this->phi = 0;
        this->frame = BODY;
    }
    Pose_t(double x, double y, double phi, frame_t frame = NED) :
        v(x,y)
    {
        this->phi = phi;
        this->frame = frame;
    }
    Pose_t(Vector2D vec, double phi, frame_t frame = NED) :
        v(vec.x, vec.y)
    {
        this->phi = phi;
        this->frame = frame;
    }
    Pose_t operator+(const Pose_t &r){
        return Pose_t(this->v.x + r.v.x, this->v.y + r.v.y, this->phi + r.phi);
    }
    Pose_t operator-(const Pose_t &r){
        return Pose_t(this->v.x - r.v.x, this->v.y - r.v.y, this->phi - r.phi);
    }
    Pose_t rotate(double angle){
        return Pose_t(v.rotate(angle), phi);
    }
} ;

#endif /* DATATYPES_H_ */
