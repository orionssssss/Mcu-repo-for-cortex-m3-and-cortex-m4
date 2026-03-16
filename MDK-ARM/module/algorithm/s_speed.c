#include "s_speed.h"
#include "stdint.h"
#include "math.h"
#include "stm32f4xx_hal.h"

S_speedControl_t S_speedControl;


void S_speedParamInit(S_speedControl_t *MoveControl)
{
    MoveControl->set_acc   = 0;
    MoveControl->set_speed = 0;
}

void S_speedDistanceCalc(S_speedControl_t *MoveControl, float dt)
{
    MoveControl->distance += MoveControl->set_speed * dt;
    MoveControl->position = MoveControl->distance / (2 * PI * RADIUS) * 360.0f + MoveControl->begin_move_angle;
}

uint8_t S_speedPreprocess(S_speedControl_t *MoveControl, double error_meter, float current_angle)
{
	static double acc_acc = 0, max_acc = 0, max_speed = 0;
    double total_error_meter = error_meter;
    MoveControl->begin_move_angle = current_angle;
    MoveControl->distance = 0;
    if (fabs(total_error_meter) >= 1) {
        MoveControl->acc_acc = ACC_ACC_01;
        MoveControl->max_acc = ACC_MAX_01;
        // MoveControl->max_speed = V_MAX_01;
        MoveControl->max_speed = sqrt(fabs(total_error_meter) * ACC_MAX_01 / 2.0f);
    } else if (fabs(total_error_meter) >= 0.1) {
        MoveControl->acc_acc = ACC_ACC_001;
        MoveControl->max_acc = ACC_MAX_001;
        // MoveControl->max_speed = V_MAX_001;
        MoveControl->max_speed = sqrt(fabs(total_error_meter) * ACC_MAX_001 / 2.0f);
    } else if (fabs(total_error_meter) >= 0.001) {
        MoveControl->acc_acc = ACC_ACC_0001;
        MoveControl->max_acc = ACC_MAX_0001;
        // MoveControl->max_speed = V_MAX_0001;
        MoveControl->max_speed = sqrt(fabs(total_error_meter) * ACC_MAX_0001 / 2.0f);
    } else {
        return 0;// 误差过小，不进行运动
    }
    // 运动时间规划
    acc_acc   = MoveControl->acc_acc;
    max_acc   = MoveControl->max_acc;
    max_speed = MoveControl->max_speed;

    MoveControl->time.t01 = (max_acc / acc_acc);
    MoveControl->time.t12 = (max_speed / max_acc) - (max_acc / acc_acc);
    MoveControl->time.t34 = (fabs(total_error_meter) / max_speed) - (max_speed / max_acc) - (max_acc / acc_acc);

    MoveControl->time.t56 = MoveControl->time.t12;

    MoveControl->time.t23 = MoveControl->time.t01;
    MoveControl->time.t45 = MoveControl->time.t01;
    MoveControl->time.t67 = MoveControl->time.t01;
    MoveControl->set_acc  = 0;
	// 总运动开始时加减速判断
    if (total_error_meter < 0) {
        MoveControl->acc_acc = -MoveControl->acc_acc;
    }
	return 1;//规划完毕，开始运动
}

uint8_t S_speedSevenSeg(S_speedControl_t *MoveControl, int32_t current_position, float dt)
{
	static float time_sum;
	switch (MoveControl->move_process_flag) {
        case 0:
		 	MoveControl->move_process_flag = MoveControl->begin_move_flag;
            MoveControl->set_acc   = 0;
            MoveControl->set_speed = 0;
            break;
        case 1:
            if (time_sum < MoveControl->time.t01) {
                MoveControl->set_acc += MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum      = 0;
                MoveControl->move_process_flag = 2;
            }
            break;
        case 2:
            if (time_sum < MoveControl->time.t12) {
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum      = 0;
                MoveControl->move_process_flag = 3;
            }
            break;
        case 3:
            if (time_sum < MoveControl->time.t23) {
                MoveControl->set_acc -= MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_speed = MoveControl->set_speed;
                time_sum               = 0;
                MoveControl->move_process_flag          = 4;
            }
            break;
        case 4:
            if (time_sum < MoveControl->time.t34) {
                MoveControl->set_speed = MoveControl->set_speed;
                time_sum += dt;
            } else {
                MoveControl->set_acc -= MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum      = 0;
                MoveControl->move_process_flag = 5;
            }
            break;
        case 5:
            if (time_sum < MoveControl->time.t45) {
                MoveControl->set_acc -= MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum      = 0;
                MoveControl->move_process_flag = 6;
            }
            break;
        case 6:
            if (time_sum < MoveControl->time.t56) {
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_acc += MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum      = 0;
                MoveControl->move_process_flag = 7;
            }
            break;
        case 7:
            if (time_sum < MoveControl->time.t67) {
                MoveControl->set_acc += MoveControl->acc_acc * dt;
                MoveControl->set_speed += MoveControl->set_acc * dt;
                time_sum += dt;
            } else {
                MoveControl->set_acc   = 0;
                MoveControl->set_speed = 0;
                time_sum               = 0;
                MoveControl->move_process_flag        = 8;
            }
            break;
        case 8:
            if (fabs(MoveControl->position - current_position) < 10)
                MoveControl->move_process_flag = 0;
				return 0;
    }
	return 1;
}

void S_speedCalc(float target_angle, float current_angle, S_speedControl_t *MoveControl, float dt)
{
    float error_pos = target_angle - current_angle;
    // encoder transform turns
    double error_turn = error_pos / 360.0f;
    // turns transform meter
    double error_meter = error_turn * 2 * PI * RADIUS;
    // record the begin error
    if (!MoveControl->begin_move_flag) {
		MoveControl->begin_move_flag = S_speedPreprocess(MoveControl, error_meter, current_angle);
    }
	else{
		S_speedDistanceCalc(MoveControl, dt);
	}
	MoveControl->begin_move_flag = S_speedSevenSeg(MoveControl, current_angle, dt);
        
}