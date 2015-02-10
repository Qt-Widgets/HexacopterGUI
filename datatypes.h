/*
 * datatypes.h
 *
 *  Created on: 08.10.2014
 *      Author: michael
 */

#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "matlib.h"

typedef struct sensor_9dof_s{
    RODOS::Vector3D accel;
    RODOS::Vector3D gyro;
    RODOS::Vector3D mag;
} Sensor_9DOF_t;

typedef struct state_s{
    RODOS::Quaternion q;
    RODOS::Vector3D w;
}State_t;

typedef struct Euler_s{
    double roll;
    double pitch;
    double yaw;
}Euler_t;

typedef struct PID_values_s{
    RODOS::Vector3D inner, outer;
    double outerLimit;
    double omegaAlpha, deltaOmegaAlpha;
}PID_values_t;

typedef struct debug_s{
    state_s state;
    PID_values_t pid;
    RODOS::Vector3D torque, error;
    RODOS::Vector6D motor;
} debug_t;

typedef struct motor_cmd_s{
    double rot1, rot2, rot3, rot4, rot5, rot6;
} motor_cmd_t;

#endif /* DATATYPES_H_ */
