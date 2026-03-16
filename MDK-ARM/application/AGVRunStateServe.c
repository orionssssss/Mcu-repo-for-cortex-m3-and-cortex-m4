#include "AGVRunStateServe.h"
#include "bsp_log.h"
#include "ParamService.h"
#include "string.h"
#include "cmsis_os2.h"


osThreadId_t AGVRunStateMachineHandle;
const osThreadAttr_t AGVRunStateMachineMain_attributes = {
  .name = "AGVRunStateMachineMain",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
void AGVRunStateMachineInit(void)
{
	AGVRunStateMachineHandle = osThreadNew(AGVRunControlMain, NULL, &AGVRunStateMachineMain_attributes);
	if(AGVRunStateMachineHandle == NULL)
		LOGERROR("RunStateMachineTask create Failed!!");
}


typedef enum{
    AUTO_RUN,
    DEBUG_RUN
}run_mode_t;

static run_mode_t   s_CurrentControlMode;

static AGVRunControlParam_t s_AGVRunControlParam;
static ControlState CurrentNodeIndex = AGV_STAGE_INIT;
const state_node_t StateNode[] = {//状态机节点对应的状态及函数
	StageInitialFuntion,
	AGV_STAGE_INIT,
    StageMotorInitFuntion,
    AGV_STAGE_MOTOR_INIT,
    StagePlatformZeroInitFuntion,
    AGV_STAGE_PLATFORM_ZERO_INIT,
	StageWaitForTaskFuntion,
	AGV_STAGE_WAIT_FOR_TASK,
	StageRunFuntion,
	AGV_STAGE_RUN,
	StageErrorFuntion,
	AGV_STAGE_ERROR
};

void GetCurrentControlMode(void)
{
    if (DEBUG_FLAG)
	{
        s_CurrentControlMode = DEBUG_RUN;
		LOGINFO("DEBUG_START");
	}
    else
	{
        s_CurrentControlMode = AUTO_RUN;
	}
    return;
}

void AGVDebug(void)
{
	;
}

void AGVAutoRun(void)//自动运行程序
{
	// AnalysisTaskToAction();
    
	static Event_e EventNum;
	if(StateNode[CurrentNodeIndex].u8StatChk == CurrentNodeIndex)
	CurrentNodeIndex = StateNode[CurrentNodeIndex].state_Function(&EventNum);
	
}
void AGVRunControlMain(void *argument)
{
	UNUSED(argument);
   /**************************************/
   for(;;)
   {
		/*参数检查*/
        ParameterLimitCheck();
		/*检查当前控制模式*/
		GetCurrentControlMode();
		switch(s_CurrentControlMode)
		{
			case DEBUG_RUN:
				AGVDebug();
				break;
			case AUTO_RUN:
				AGVAutoRun();
				break;
			default:break;
		}

		/*输出结果到动力模块*/

		/*输出结果到LED控制器*/

		/*输出结果到语音模块*/

		osDelay(1000);
    }
	
}


ControlState StageInitialFuntion(Event_e* pEvnt)
{
	ControlState mCurrentStage = (ControlState)0;
	switch (*pEvnt)
	{
	case AGV_EVENT_ERROR:
		/* code */
		break;
	default:
	/*启动检测插枪：如插枪则报错*/

	/*延时状态切换 更新触发事件*/
	s_AGVRunControlParam.voice_param.voice_id = VOICE_PLAY_OFFLINE;
;
	mCurrentStage = AGV_STAGE_MOTOR_INIT;
		break;
	}
	

	return mCurrentStage;
}

ControlState StageMotorInitFuntion(Event_e* pEvnt)
{
    ControlState mCurrentStage = (ControlState)0;
    /*语音设置*/
	s_AGVRunControlParam.voice_param.voice_id = VOICE_PLAY_INIT_STATE;
    /*灯状态设置*/
    
    /*电机状态初始化*/
    //MotorEnable();
    /*状态切换*/
    mCurrentStage = AGV_STAGE_PLATFORM_ZERO_INIT;
   
	return mCurrentStage;
}

/*ï¿½ï¿½ï¿½ß³ï¿½Ê¼ï¿½ï¿½*/
ControlState StagePlatformZeroInitFuntion(Event_e* pEvnt)
{
	/*设置语音*/

	/*设置灯*/

	/*设置控制参数*/
	switch(*(ControlState*)pEvnt)
	{
		case AGV_STAGE_MOTOR_INIT:
			break;
		default:
			break;
	}
	return *(ControlState*)pEvnt;
}
/*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½È´ï¿½ï¿½ï¿½ï¿½ï¿½*/
ControlState StageWaitForTaskFuntion(Event_e* pEvnt)
{
	ControlState mCurrentStage = (ControlState)0;
	return mCurrentStage;
}

ControlState  StageRunFuntion(Event_e* pEvnt)
{
	ControlState mCurrentStage = (ControlState)0;
	return mCurrentStage;
}

ControlState  StageErrorFuntion(Event_e* pEvnt)
{
	ControlState mCurrentStage = (ControlState)0;
	return mCurrentStage;
}




/************************get all state and param********************************/
ControlState GetNowState(void)
{
	return CurrentNodeIndex;
}
RunState_e GetNowRunState(void)
{
	return s_AGVRunControlParam.run_state;
}
GunState_e GetNowGunState(void)
{
	return s_AGVRunControlParam.gun_state;
}

ErrorState_e GetNowErrorState(void)
{
	return s_AGVRunControlParam.error_state;
}
LedParam_t  GetNowRGBPlayType(void)
{
	return s_AGVRunControlParam.led_param;
}
VoiceID_e GetNowVoicePlayID(void)
{
	return s_AGVRunControlParam.voice_param.voice_id;
}