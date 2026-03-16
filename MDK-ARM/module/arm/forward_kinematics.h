#ifndef forward_kinematics_h__
#define forward_kinematics_h__

#include "math.h"


#define PI 3.1415926535897932384626433832795

#define NUM_JOINTS 5
#define NUM_WRIST_JOINTS 3

#define L1 0.9
#define L2 2
#define L3 2
#define L4 2
#define L5 0.2
#define L6 2

typedef struct {
    float alpha;
    float a;
    float d;
    float theta;
} DH_Param_t;

typedef enum{
    JOINT_1 = 0,
    JOINT_2,
    JOINT_3,
    JOINT_4,
    JOINT_5,
    JOINT_6
} JointIndex_e;

typedef enum{
    RX = 0,
    TX,
    RZ,
    TZ
} DH_ParamIndex_e;

typedef struct {
    double x;
    double y;
    double z;
    double pitch;
    double roll;
    double yaw;
} TargetParam_t;



void Rotate_z(double theta, double Rz[4][4]);
void Trans_z(double d, double Tz[4][4]);
void Rotate_x(double alpha, double Rx[4][4]);
void Rotate_y(double alpha, double Ry[4][4]);
// void HomogeneousTransformation(DH_Param_t DH_Param, JointIndex_e JointIndex, Matrix *H_matrix);
void InverseKinematics(double H_matrix[4][4], DH_Param_t DH_Param[NUM_JOINTS+1], double joint_angles[NUM_JOINTS], TargetParam_t *target);
// void ForwardKinematics(DH_Param_t DH_Param[NUM_JOINTS+1], Matrix *H_matrix[NUM_JOINTS+1], uint8_t num_joints);

#endif