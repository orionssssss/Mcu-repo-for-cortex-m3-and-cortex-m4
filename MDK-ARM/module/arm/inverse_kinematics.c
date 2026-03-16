#include "inverse_kinematics.h"
#include "stdint.h"
#include "forward_kinematics.h"
// #include "matrix.h"

// void InverseKinematics(double *H_matrix, DH_Param_t DH_Param[NUM_JOINTS+1], double joint_angles[NUM_JOINTS], TargetParam_t *target)
// {
//     // Placeholder for inverse kinematics calculations
//     // This function should compute the joint angles required to achieve the desired end-effector pose represented by H_matrix
//     // The implementation will depend on the specific robot configuration and kinematic equations
//     /*求解腕部关节位置*/
//     TargetParam_t * wrist_target;
//     double rotate_temp[4][4] = Rotate_z(target->yaw, )*Rotate_y(target->pitch)*Rotate_x(target->roll);
//     //旋转矩阵
//     //齐次变换矩阵
//     /*求解3轴位置关节角度*/
    
//     double q1 = -atan2(wrist_target->x, wrist_target->y);
//     joint_angles[0] = q1;
//     double r = sqrt(wrist_target->x*wrist_target->x + wrist_target->y*wrist_target->y);
//     double h = wrist_target->z - L1;
//     double q3 = -acosf(h*h + r*r - L2*L2 - L3*L3)/(2*L2*L3);
//     double a = L2 + L3*cos(q3);
//     double q3_temp = fabs(q3);
//     double q2 = atan2(h, r) + atan2(L3*sin(q3_temp), L2 + L3*cos(q3_temp));
//     /*求解姿态关节角*/
// }
