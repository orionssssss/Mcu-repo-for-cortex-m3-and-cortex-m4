#include "ServiceDriver.h"
#include "ParamService.h"
#include "snr9816vr.h"
#include "modbus.h"
#include "cmsis_os2.h"
#include "bsp_log.h"
#include "modbus_slave.h"
#include "forward_kinematics.h"

osThreadId_t ServiceDriverMainHandle;
const osThreadAttr_t ServiceDriverMain_attributes = {
  .name = "ServiceDriverMain",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
void ServiceDriverInit(void)
{
	// Initialize resources needed for the service driver here
	ServiceDriverMainHandle = osThreadNew(ServiceDriverMain, NULL, &ServiceDriverMain_attributes);
	if(ServiceDriverMainHandle == NULL)
		LOGERROR("ServiceDriverMainTask create Failed!!");
}
/*五轴机械臂参数初始化begin*/
double Matrix[4][4] = {0};
DH_Param_t DH_Param[NUM_JOINTS+1] = {
    {0.0,     0.0,  L1,  PI/2.0},   // Joint 1
    {PI/2.0, 0.0f,  0.0, 0.0   },   // Joint 2
    {0.0,     L2,   0.0, 0.0   },   // Joint 3
    {0.0,     L3,   0.0, PI/2.0},   // Joint 4
    {PI/2.0,  0.0, 0.0,  PI    },   // Joint 5
    {0.0,     0.0, L4+L5, 0.0  }    // END
};
double JointAngle[NUM_JOINTS] = {0.0, 0.0, 0.0, 0.0, 0.0};
TargetParam_t TargetParam = {-0.5, 0.5, 3.0, PI/4.0, 0.0, 0.0};
/*五轴机械臂参数初始化end*/
uint32_t CountData[8];
void ServiceDriverMain(void *argument)
{
	UNUSED(argument);
	LOG("ServiceTaskStart");
 	Snr9816vrConfig_t snr9816vr_config={
		.uart_init = {
			.uart_id = &huart2
		},
		.volume = 1,
		.speed = 1
	};
	Snr9816vr_t *VoiceHandle = Snr9816ttsRegister(&snr9816vr_config);
	InitTouchScreen();
	
	for(;;)
	{
		// HAL_UART_Transmit_IT(&huart3, (uint8_t *)&RxData, 6);
		ModbusDealData();
		SNR9816TTSVoicePlayMain();
		/*五轴机械臂逆运动学运算测试*/
		// CountData[1] = HAL_GetTick();
		// InverseKinematics(Matrix, DH_Param, JointAngle, &TargetParam);
		// CountData[2] = HAL_GetTick() - CountData[1];
		// CountData[0]++;
		/*五轴机械臂逆运动学运算测试*/
		osDelay(1);
	}
}
