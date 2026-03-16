#include "power_module.h"
#include "hjpushbeam.h"
#include "pushbeam.h"
#include "s_speed.h"
#include "modbus.h"
#include "bsp_log.h"
#include "cmsis_os2.h"

int32_t motor_position = 0;
SitaiweiInstance_s *Motor;
LingKongInstance_s *LingKongMotor;

/*taskparam*/
osThreadId_t MotorControlMainHandle;
const osThreadAttr_t MotorControlMain_attributes = {
  .name = "MotorControlMain",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t PushbeamControlMainHandle;
const osThreadAttr_t PushbeamControlMain_attributes = {
  .name = "PushbeamControlMain",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/*taskparam*/

void PowerModuleInit(void)
{
    MotorControlMainHandle = osThreadNew(MotorControlMain, NULL, &MotorControlMain_attributes);
	PushbeamControlMainHandle = osThreadNew(PushbeamControlMain, NULL, &PushbeamControlMain_attributes);
	if(MotorControlMainHandle == NULL)
		LOGERROR("MotorControlMainTask create Failed!!");
	if(PushbeamControlMainHandle == NULL)
		LOGERROR("PushbeamControlMainTask create Failed!!");
}


// void MotorControlMain(void *argument)
// {
//     UNUSED(argument);
// 	LOG("MotorControlTask Start");
//     //   VOFA_Init();
//     S_speedParamInit(&S_speedControl);
//     /*注锟斤拷一锟斤拷锟斤拷泰锟斤拷锟斤拷锟?*/
//     SitaiweiConfig_t SitaiweiConfig = {
//         .can_config.hcan_id          = &hcan1,
//         .can_config.master_id        = 0x01,
//         .can_config.receiver_id      = 0x01,
// 	};
//     Motor = SiTaiWeiMotorRegitster(&SitaiweiConfig);
//     ReadVersion(Motor);
//     for (;;) {
//         if (S_speedControl.position >= 0 && motor_position == 0) {
//             motor_position = 1;
//             osDelay(10);
//         } else if (S_speedControl.position <= -1639 && motor_position == 1) {
//             motor_position = 0;
//             osDelay(10);
//         }
//         switch (motor_position) {
//             case 0:
//                 S_speedCalc(0, Motor->total_position, &S_speedControl, 0.001f);
//                 break;
//             case 1:
//                 S_speedCalc(-1639, Motor->total_position, &S_speedControl, 0.001f);
//                 break;
//             default:
//                 break;
//         }
//         Motor->SetPosition(Motor, S_speedControl.position);
//         // VOFA_SEND_8(S_speedControl.position,  S_speedControl.set_acc, S_speedControl.set_speed, Motor->total_position, \
//     //             S_speedControl.max_speed, motor_position, data[9], data[10]);
//         osDelay(1);
//     }
// }

void MotorControlMain(void *argument)
{
    UNUSED(argument);
	LOG("MotorControlTask Start");
    S_speedParamInit(&S_speedControl);
    /*注册一个伺泰威电机*/
	
    LingKongConfig_t LingKongConfig = {
        .can_config.hcan_id          = &hcan2,
        .can_config.master_id        = 0x01,
        .can_config.receiver_id      = 0x01,
	};
	LingKongMotor = LingKongMotorRegitster(&LingKongConfig);
    // Motor = SiTaiWeiMotorRegitster(&SitaiweiConfig);
    // ReadVersion(Motor);//读版本
	LingKongMotor->clearerr(LingKongMotor);//使能电机
	osDelay(1);
	LingKongMotor->enable(LingKongMotor);//清除错误
	osDelay(1);
	// LingKongMotor->SetSpeed(LingKongMotor, 80000);//位置归零
	// LingKongMotor->SetSinglePosition(LingKongMotor, 1, LingKongMotor->control.set_total_angle * 10);//位置归零
    for (;;) {
		GetTotalAngle(LingKongMotor);
        if (S_speedControl.position <= 0.01f && motor_position == 0) {
            motor_position = 1;
            osDelay(10);
        } else if (S_speedControl.position >= 19.99f && motor_position == 1) {
            motor_position = 0;
            osDelay(10);
        }
        switch (motor_position) {
            case 0:
                S_speedCalc(0, LingKongMotor->total_angle, &S_speedControl, 0.001f);
                break;
            case 1:
                S_speedCalc(20, LingKongMotor->total_angle, &S_speedControl, 0.001f);
                break;
            default:
                break;
        }
	LingKongMotor->SetPosition(LingKongMotor, S_speedControl.position * 1000);
    osDelay(1);
    }
}

static uint16_t pushbeam_datasend1[13];
static uint16_t pushbeam_datasend2[13];
static uint8_t pushbeam_flag = 0;

// void PushbeamControlMain(void *argument)
// {
//     UNUSED(argument);
// 	LOG("PushbeamControlTask Start");
// 	//uart config
// 	KBSPushbeamConfig_t pushbeam1_config = {
// 		.modbus_config = {
// 			.name = "modbus1", 
// 			.slave_addr = 1, 
// 			.uart_config = {
// 				.uart_id = &huart6
// 			}
// 		},
// 		.name = "pushbeam1",
// 	};
// 	KBSPushbeamInstance_t * pushbeam1 = KBSPushbeamRegister(&pushbeam1_config);
// 	/*??2???*/
// 	KBSPushbeamConfig_t pushbeam2_config = {
// 		.modbus_config = {
// 			.name = "modbus2", 
// 			.slave_addr = 1, 
// 			.uart_config = {
// 				.uart_id = &huart7
// 			}
// 		},
// 		.name = "pushbeam2",
// 	};
// 	KBSPushbeamInstance_t * pushbeam2 = KBSPushbeamRegister(&pushbeam2_config);
// 	/*使能*/
// 	// pushbeam1->enable(pushbeam1);
// 	// pushbeam2->enable(pushbeam2);
// 	/*获取数据*/
// 	pushbeam1->getdata(pushbeam1);
// 	pushbeam2->getdata(pushbeam2);
// 	for(;;)
// 	{
// 		pushbeam_flag++;
// 		if(pushbeam_flag >= 10){
// 			pushbeam1->getdata(pushbeam1);
// 			pushbeam2->getdata(pushbeam2);
// 			if(pushbeam1->pushbeam_t.data.init == 0 || pushbeam2->pushbeam_t.data.init == 0){
// 				// pushbeam1->enable(pushbeam1);
// 				// pushbeam2->enable(pushbeam2);
// 			}
// 		}
// 		if (pushbeam_flag >= 5){
// 			pushbeam_datasend1[2] = 0;
// 			pushbeam_datasend2[2] = 0;
// 			pushbeam_flag %= 10;
// 		}
// 		else if(pushbeam_flag <= 5){
// 			pushbeam_datasend1[2] = 100;
// 			pushbeam_datasend2[2] = 0;
// 		}
// 		pushbeam1->setposition(pushbeam1, pushbeam_datasend1[2], 100);
// 		pushbeam2->setposition(pushbeam2, pushbeam_datasend2[2], 100);
		
		

// 		osDelay(10);
// 	}
// }

uint16_t look[10];
void PushbeamControlMain(void *argument)
{
    UNUSED(argument);
	LOG("PushbeamControlTask Start");
	//uart config
	HJPushbeamConfig_t pushbeam1_config = {
		.modbus_config = {
			.name = "modbus1", 
			.crc_type = MODBUS_CRC_LSBFITST,
			.slave_addr = 1, 
			.uart_config = {
				.uart_id = &huart6
			}
		},
		.name = "pushbeam3",
	};
	HJPushbeamInstance_t * pushbeam1 = HJPushbeamRegister(&pushbeam1_config);
	static uint8_t set_speed = 100, check_set_speed, now_speed;
	static uint16_t wait_count, error_count;
	// pushbeam1->SetRunParam(pushbeam1, 0, 0, 0);
	/*获取数据*/
	pushbeam1->getdata(pushbeam1);
	for(;;)
	{
		
		// pushbeam_flag++;
		// if(pushbeam_flag >= 10){
		// 	pushbeam1->getdata(pushbeam1);
		// }
		// if (pushbeam_flag >= 5){
		// 	pushbeam_datasend1[2] = 0;
		// 	pushbeam_flag %= 10;
		// }
		// else if(pushbeam_flag <= 5){
		// 	pushbeam_datasend1[2] = 100;
		// }
		// pushbeam1->setposition(pushbeam1, pushbeam_datasend1[2], 80);
		// osDelay(500);

		wait_count++;
		if(wait_count >= 1)
		{
			// wait_count = 0;
			pushbeam_flag++;
			if(pushbeam_flag < 20){
				// pushbeam_datasend1[2] = 1000;
				set_speed -= 5;//疑似速度变化不能小于5 否侧出现通信出错的问题
			}
			else if (pushbeam_flag >= 19){
				// pushbeam_datasend1[2] = 0;
				set_speed += 5;
			}
			pushbeam_flag %= 38;
			if(wait_count == 500 && pushbeam_datasend1[2] == 1000)
			{
				pushbeam_datasend1[2] = 0;
				wait_count = 0;
			}
			else if (wait_count == 500 && pushbeam_datasend1[2] == 0)
			{
				pushbeam_datasend1[2] = 1000;
				wait_count = 0;
			}
			
			pushbeam1->setposition(pushbeam1, pushbeam_datasend1[2], set_speed);
			// if(pushbeam_flag >= 10){
			// }
		}
		pushbeam1->getdata(pushbeam1);
		if(pushbeam1->modbus_instance->state != MODBUS_SUCCESS)
		{
			error_count++;
		}
		check_set_speed = pushbeam1->pushbeam_t.data.set_speed;
		now_speed = pushbeam1->pushbeam_t.data.current_speed;
		osDelay(1);


		// osDelay(50);
	}
}