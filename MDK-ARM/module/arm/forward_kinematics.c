#include "forward_kinematics.h"
#include "matrix.h"
#include "stdint.h"



void Rotate_z(double theta, double Rz[4][4])
{
    double temp1 = cos(theta);
    double temp2 = sin(theta);
    Rz[0][0] = temp1;Rz[0][1] = -temp2;Rz[0][2] = 0;Rz[0][3] = 0;
    Rz[1][0] = temp2;Rz[1][1] = temp1;Rz[1][2] = 0;Rz[1][3] = 0;
    Rz[2][0] = 0;Rz[2][1] = 0;Rz[2][2] = 1;Rz[2][3] = 0;
    Rz[3][0] = 0;Rz[3][1] = 0;Rz[3][2] = 0;Rz[3][3] = 1;
}
void Trans_z(double d, double Tz[4][4])
{
    Tz[0][0] = 1;Tz[0][1] = 0;Tz[0][2] = 0;Tz[0][3] = 0;
    Tz[1][0] = 0;Tz[1][1] = 1;Tz[1][2] = 0;Tz[1][3] = 0;
    Tz[2][0] = 0;Tz[2][1] = 0;Tz[2][2] = 1;Tz[2][3] = d;
    Tz[3][0] = 0;Tz[3][1] = 0;Tz[3][2] = 0;Tz[3][3] = 1;
}

void Trans_x(double a, double Tx[4][4])
{
    Tx[0][0] = 1;Tx[0][1] = 0;Tx[0][2] = 0;Tx[0][3] = a;
    Tx[1][0] = 0;Tx[1][1] = 1;Tx[1][2] = 0;Tx[1][3] = 0;
    Tx[2][0] = 0;Tx[2][1] = 0;Tx[2][2] = 1;Tx[2][3] = 0;
    Tx[3][0] = 0;Tx[3][1] = 0;Tx[3][2] = 0;Tx[3][3] = 1;
}

void Rotate_x(double alpha, double Rx[4][4])
{
    Rx[0][0] = 1;Rx[0][1] = 0;Rx[0][2] = 0;Rx[0][3] = 0;
    Rx[1][0] = 0;Rx[1][1] = cos(alpha);Rx[1][2] = -sin(alpha);Rx[1][3] = 0;
    Rx[2][0] = 0;Rx[2][1] = sin(alpha);Rx[2][2] = cos(alpha);Rx[2][3] = 0;
    Rx[3][0] = 0;Rx[3][1] = 0;Rx[3][2] = 0;Rx[3][3] = 1;
}

void Rotate_y(double alpha, double Ry[4][4])
{
    Ry[0][0] = cos(alpha);Ry[0][1] = 0;Ry[0][2] = sin(alpha);Ry[0][3] = 0;
    Ry[1][0] = 0;Ry[1][1] = 1;Ry[1][2] = 0;Ry[1][3] = 0;
    Ry[2][0] = -sin(alpha);Ry[2][1] = 0;Ry[2][2] = cos(alpha);Ry[2][3] = 0;
    Ry[3][0] = 0;Ry[3][1] = 0;Ry[3][2] = 0;Ry[3][3] = 1;
}
Matrix *mat_Rz_1;
Matrix *mat_Rx_1;
Matrix *mat_Tz_1;
Matrix *mat_Tx_1;
Matrix *matrix[NUM_JOINTS][4];
double H[4][4];
Matrix * HomogeneousTransformation(DH_Param_t DH_Param, JointIndex_e JointIndex)
{
    double Rz[4][4];
    double Rx[4][4];
    double Tz[4][4];
    double Tx[4][4];
    Rotate_z(DH_Param.theta, Rz);
    Rotate_x(DH_Param.alpha, Rx);
    Trans_z(DH_Param.d, Tz);
    Trans_x(DH_Param.a, Tx);
    // double *mat_Rz;
    matrix[JointIndex][RZ] = Matrix_gen(4,4,&Rz[0][0]);
    matrix[JointIndex][RX] = Matrix_gen(4,4,&Rx[0][0]);
    matrix[JointIndex][TZ] = Matrix_gen(4,4,&Tz[0][0]);
    matrix[JointIndex][TX] = Matrix_gen(4,4,&Tx[0][0]);
    Matrix * Matrix_temp;
    Matrix * Matrix_temp1;
    Matrix * Matrix_temp2;
    Matrix_temp = M_mul(matrix[JointIndex][RX], matrix[JointIndex][TX]);
    Matrix_temp1 = M_mul(Matrix_temp, matrix[JointIndex][RZ]);
    Matrix_temp2 = M_mul(Matrix_temp1, matrix[JointIndex][TZ]);
    // for (int i = 0; i < Matrix_temp->row; i++) {
    //     for (int j = 0; j < Matrix_temp->column; j++) {
    //         H[i][j] = Matrix_temp->data[i * (Matrix_temp->column) + j];
    //     }
    // }
    // H_matrix = Matrix_temp2;
    
    /*释放内存*/
    M_free(matrix[JointIndex][RZ]);
    M_free(matrix[JointIndex][RX]);
    M_free(matrix[JointIndex][TZ]);
    M_free(matrix[JointIndex][TX]);
    M_free(Matrix_temp);
    M_free(Matrix_temp1);
    return Matrix_temp2;
}
/**
 * @brief  : 正运动学计算函数
 * @param  : DH_Param: DH参数表
 * @param  : H_matrix: 矩阵数组，每个关节的齐次变换矩阵
 * @param  : num_joints: DH参数表行数
 * @retval : None
 */
void ForwardKinematics(DH_Param_t DH_Param[NUM_JOINTS+1], Matrix *H_matrix[NUM_JOINTS+1], uint8_t num_joints)
{
    /*获取底座到末端的DH参数表矩阵*/

    /*计算末端姿态*/
}

void InverseKinematics(double H_matrix[4][4], DH_Param_t DH_Param[NUM_JOINTS+1], double joint_angles[NUM_JOINTS], TargetParam_t *target)
{
    // Placeholder for inverse kinematics calculations
    // This function should compute the joint angles required to achieve the desired end-effector pose represented by H_matrix
    // The implementation will depend on the specific robot configuration and kinematic equations
    /*求解腕部关节位置*/
    TargetParam_t wrist_target;//腕关节位置结构体
    double end_Rx[4][4];
    double end_Rz[4][4];
    double end_Ry[4][4];

    target->yaw = -atan2(target->y,target->x); //舍弃yaw轴获取更加多的解。
    Rotate_z(target->yaw, end_Rz);
    Rotate_y(target->pitch, end_Ry);
    Rotate_x(target->roll, end_Rx);
    //计算齐次变换矩阵
    Matrix * end_Rx_Matrix = Matrix_gen(4,4,&end_Rx[0][0]);
    Matrix * end_Ry_Matrix = Matrix_gen(4,4,&end_Ry[0][0]);
    Matrix * end_Rz_Matrix = Matrix_gen(4,4,&end_Rz[0][0]);
    Matrix * Matrix_temp0 = M_mul(end_Rx_Matrix, end_Ry_Matrix);//提取工具的齐次变换矩阵
    Matrix * Matrix_temp = M_mul(Matrix_temp0, end_Rz_Matrix);
    /*释放末端矩阵内存*/
    M_free(end_Rx_Matrix);
    M_free(end_Ry_Matrix);
    M_free(end_Rz_Matrix);
    M_free(Matrix_temp0);
    //提取工具坐标系z轴方向向量
    double z_tool[3] = {Matrix_temp->data[0 * (Matrix_temp->column) + 2], \
        Matrix_temp->data[1 * (Matrix_temp->column) + 2], \
        Matrix_temp->data[2 * (Matrix_temp->column) + 2]};
    //计算腕部位置
    double L = L4+L5; //工具长度
    wrist_target.x = target->x - z_tool[0]*L;
    wrist_target.y = target->y - z_tool[1]*L;
    wrist_target.z = target->z - z_tool[2]*L;

    /*求解3轴位置关节角度*/
    double q1 = -atan2(wrist_target.x, wrist_target.y);
    double r = sqrt(wrist_target.x*wrist_target.x + wrist_target.y*wrist_target.y);
    double h = wrist_target.z - L1;
    double q3 = -acosf((h*h + r*r - L2*L2 - L3*L3)/(2*L2*L3));
    double a = L2 + L3*cos(q3);
    double q3_temp = fabs(q3);
    double q2 = atan2(h, r) + atan2(L3*sin(q3_temp), L2 + L3*cos(q3_temp));

    /*求解姿态关节角*/
    Matrix *H_matrix_Matrix[NUM_WRIST_JOINTS+1];
    DH_Param_t DH_Param_temp[NUM_WRIST_JOINTS+1];
    memcpy(DH_Param_temp, DH_Param, sizeof(DH_Param_t)*(NUM_WRIST_JOINTS+1));
    DH_Param_temp[0].theta += q1;
    DH_Param_temp[1].theta += q2;
    DH_Param_temp[2].theta += q3;
    /*获取底座到腕部的DH参数表矩阵*/
    for(int i = 0; i <= NUM_WRIST_JOINTS; i++) {
        H_matrix_Matrix[i] = HomogeneousTransformation(DH_Param_temp[i], (JointIndex_e)i);
    }
    /*计算到腕部总的变换矩阵，机械臂DH参数旋转应该是内旋，所以用右乘*/
    Matrix *TransWrist_temp1 = M_mul(H_matrix_Matrix[0], H_matrix_Matrix[1]);
    Matrix *TransWrist_temp2 = M_mul(TransWrist_temp1, H_matrix_Matrix[2]);
    Matrix *TransWrist_temp3 = M_mul(TransWrist_temp2, H_matrix_Matrix[3]);
    /*释放前三轴三次转换的腕关节的齐次变换坐标矩阵的内存*/
    for(int i = 0; i <= NUM_WRIST_JOINTS; i++) {
        M_free(H_matrix_Matrix[i]);
    }
    // for (int i = 0; i < TransWrist->row; i++) {
    //     for (int j = 0; j < TransWrist->column; j++) {
    //         H[i][j] = TransWrist->data[i * (TransWrist->column) + j];
    //     }
    // }
    Matrix *TransWrist = M_T(TransWrist_temp3);
    Matrix *wrist_need_matrix = M_mul(TransWrist, Matrix_temp);
    double q4 = atan2(wrist_need_matrix->data[1 * (wrist_need_matrix->column) + 0], wrist_need_matrix->data[0 * (wrist_need_matrix->column) + 0]);
    double q5 = atan2(-wrist_need_matrix->data[1 * (wrist_need_matrix->column) + 1], wrist_need_matrix->data[1 * (wrist_need_matrix->column) + 0]);
    // for (int i = 0; i < wrist_need_matrix->row; i++) {
    //     for (int j = 0; j < wrist_need_matrix->column; j++) {
    //         H_matrix[i][j] = wrist_need_matrix->data[i * (wrist_need_matrix->column) + j];
    //     }
    // }
    joint_angles[JOINT_1] = q1;
    joint_angles[JOINT_2] = q2;
    joint_angles[JOINT_3] = q3;
    joint_angles[JOINT_4] = q4;
    joint_angles[JOINT_5] = q5;
     /*释放内存*/
    
    
    M_free(Matrix_temp);
    M_free(TransWrist_temp1);
    M_free(TransWrist_temp2);
    M_free(TransWrist_temp3);
    M_free(TransWrist);
    M_free(wrist_need_matrix);
}
