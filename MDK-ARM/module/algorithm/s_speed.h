#ifndef _S_SPEED_H__
#define _S_SPEED_H__
#include "stdint.h"

#define PI 3.1415926535898f
#define RADIUS 0.02f
#define ENCODER_COUNTS_PER_REVOLUTION 16384.0f

/**三项参数由t34决定 t34需要大于等于0**/
//0.1m
#define ACC_ACC_01 5.0f
#define ACC_MAX_01 2.0f
#define V_MAX_01 1.0f
//0.01m
#define ACC_ACC_001 40.0f
#define ACC_MAX_001 2.0f
#define V_MAX_001 0.2f
//0.001m
#define ACC_ACC_0001 0.4f
#define ACC_MAX_0001 0.02f
#define V_MAX_0001 0.01f
//计算acc_acc和Amax时，需满足Vmax*Acc_Acc > pow(Amax, 2)保证存在7段式梯形规划
//计算Vmax = sqrt(X*Amax/2)使匀速段和加减速段时间相等
//根据以上内容可以得到：X*pow(Acc_Acc, 2) > 2*pow(Amax, 3)，以此公式进行调参


typedef struct{
	double t01;
	double t12;
	double t23;
	double t34;
	double t45;
	double t56;
	double t67;
	
	float t_middle;
	float t_total;
}time_t;

typedef struct{
	double max_speed;
	double current_speed;
	double target_speed;
	double set_speed;
	double max_acc;
	double acc_acc;
	double set_acc;
	
	double distance;
	float position;
	time_t time;

	double begin_move_angle;
	uint8_t begin_move_flag;
	uint8_t move_process_flag;
	
}S_speedControl_t;//单位以m为准
	
extern S_speedControl_t S_speedControl;

 /**
  * @brief 初始化结构体
  * 
  * @param MoveControl 
  */
void S_speedParamInit(S_speedControl_t *MoveControl);
/**
  * @brief 计算速度
  * 
  * @param target_position 目标位置
  * @param current_position 当前位置
  * @param MoveControl 速度结构体
  * @param dt 时间间隔
  */
void S_speedCalc(float target_position, float current_position, S_speedControl_t *MoveControl, float dt);

#endif
