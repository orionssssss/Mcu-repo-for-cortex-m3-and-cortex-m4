#include "snr9816vr.h"
#include "AGVRunStateServe.h"
#include "cmsis_os2.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

#define OsDelay  osDelay


uint8_t RegisterFinishFlag;//注册完成标志
Snr9816vr_t * SNR9816TTSModule;//注册的语音模块实例
static char Snr9816data[1024];
static uint8_t s_MusicSendData[1024];
static VoiceID_e s_VoicePlayState;

uint8_t mPlayStage;
float mSendCommandTimeCount;
// static char MusicData[1024];


float mVoicePlayFreeCycleTimeCount;
uint16_t Com_VoicePlayCycleTime = 5000;


void snr9816tts_send_command(Snr9816vrCommand_e cmd, int parameter, char *sentence)
{
	uint32_t dat_len = 0, sentence_len = strlen(sentence);
	char MusicSendDataTmp[20] = {0};
	switch (cmd)
	{
	case COMMAND_SET_VOLUME:
		
		sprintf(MusicSendDataTmp, "[v%d]", parameter);
		dat_len = strlen(MusicSendDataTmp);
		s_MusicSendData[0] = 0xFD;
		dat_len = dat_len + 2;
		s_MusicSendData[1] = (dat_len >> 8);
		s_MusicSendData[2] = (dat_len);
		s_MusicSendData[3] = (0x01);
		s_MusicSendData[4] = (0x01);
		dat_len = strlen((char *)MusicSendDataTmp);
		for (int i = 0; i < dat_len; i++)
			s_MusicSendData[5 + i] = MusicSendDataTmp[i];
		dat_len = 5 + dat_len;
		HAL_UART_Transmit_IT(&huart2, &s_MusicSendData[0], dat_len);
		break;
	case COMMAND_SET_SPEED:
		s_MusicSendData[0] = 0xFD;
		s_MusicSendData[1] = 0x00;
		s_MusicSendData[2] = 0x06;
		s_MusicSendData[3] = (0x01);
		s_MusicSendData[4] = (0x01);
		s_MusicSendData[5] = 0x5B;
		s_MusicSendData[6] = 0x73;
		s_MusicSendData[7] = 0x30;
		s_MusicSendData[8] = 0x5D;
		if (parameter >= 0 && parameter <= 9)
		{
			s_MusicSendData[7] += parameter;
			HAL_UART_Transmit_IT(&huart2, &s_MusicSendData[0], dat_len);
		}
		break;
	case COMMAND_CHECK_STATE:
		s_MusicSendData[0] = 0xFD;
		s_MusicSendData[1] = 0x00;
		s_MusicSendData[2] = 0x01;
		s_MusicSendData[3] = 0x21;
		dat_len = 4;
		HAL_UART_Transmit_IT(&huart2, &s_MusicSendData[0], dat_len);
		break;
	case COMMAND_STOP_PLAY: // ??????±¨
		s_MusicSendData[0] = 0xFD;
		s_MusicSendData[1] = 0x00;
		s_MusicSendData[2] = 0x01;
		s_MusicSendData[3] = 0x02;
		dat_len = 4;
		HAL_UART_Transmit_IT(&huart2, &s_MusicSendData[0], dat_len);
		break;
	case COMMAND_PLAY_VOICE: // ??±¨?ù??
		s_MusicSendData[0] = 0xFD;
		dat_len = sentence_len + 2;
		s_MusicSendData[1] = (dat_len >> 8);
		s_MusicSendData[2] = (dat_len);
		s_MusicSendData[3] = (0x01);
		s_MusicSendData[4] = (0x01);
		for (int i = 0; i < sentence_len; i++)
			s_MusicSendData[5 + i] = sentence[i];
		dat_len = 5 + sentence_len;
		HAL_UART_Transmit_IT(&huart2, &s_MusicSendData[0], dat_len);
		mVoicePlayFreeCycleTimeCount = 0;
		break;
	default:
		break;
	}

	mSendCommandTimeCount = 0;
}


void SNR9816TTSVoicePlayMain(void)
{
	/*判断状态更新*/
	VoiceID_e newPlayType = GetNowVoicePlayID();
	if (s_VoicePlayState != newPlayType)
	{
		s_VoicePlayState = newPlayType;
		/*先终止当前语音再进行播放新语音*/
		snr9816tts_send_command(COMMAND_STOP_PLAY, 0, 0);
	}
	OsDelay(50);
	if(mVoicePlayFreeCycleTimeCount++ >= Com_VoicePlayCycleTime)
	{//以规定的周期播放语音
		mVoicePlayFreeCycleTimeCount = 0;
		switch (s_VoicePlayState)
		{
		case VOICE_NULL:
			break;
		case VOICE_PLAY_INIT_STATE:
				// sprintf(MusicData, "设备正在初始化,请稍候");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("设备正在初始化,请稍候");
			break;
		case VOICE_PLAY_STATION:
				/*获取目标站点*/
				// Snr9816ttsSetVoice("设备正在前往%d号站点,请耐心等待", mPlayDestationNo);
			break;
		case VOICE_PLAY_REPOSITION:
				// sprintf(MusicData, "设备正在取点，请耐心等待");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("设备正在取电，请耐心等待");
			break;
		case VOICE_PLAY_ERROR:
				/*获取错误码*/
				//snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, GetErrorString(mPlayErrorCode));
			break;
		case VOICE_PLAY_OFFLINE:
				// sprintf(MusicData, "设备当前处于离线状态,正在努力连接中");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("设备当前处于离线状态,正在努力连接中");
			break;
		case VOICE_ROLL_UP:
				// sprintf(MusicData, "设备正在回收枪线,请勿拖拽枪线");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("设备正在回收枪线,请勿拖拽枪线");
			break;
		case VOICE_ROLL_DOWN: 
				// sprintf(MusicData, "枪线线正在下放,请勿拖拽枪线");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("枪线线正在下放,请勿拖拽枪线");
			break;
		case VOICE_INSERT: 
				// sprintf(MusicData, "枪线下降完毕，请插枪");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("枪线下降完毕，请插枪");
			break;
		case VOICE_INSERT_OK:
				// sprintf(MusicData, "枪线插入完毕，请在小程序付费开始充电");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("枪线插入完毕，请在小程序付费开始充电");
			break;
		case VOICE_CHARGE_BEGIN:
				// sprintf(MusicData, "开始充电");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("开始充电");
			break;
		case VOICE_EXTRACT: 
				// sprintf(MusicData, "充电已完成，请拔枪");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("充电已完成，请拔枪");
			break;
		case VOICE_EXTRACT_OK: 
				// sprintf(MusicData, "已拔枪,15秒后枪线自动回收,感谢您使用小升充电");
				// snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, MusicData, strlen(MusicData));
				Snr9816ttsSetVoice("已拔枪,15秒后枪线自动回收,感谢您使用小升充电");
			break;
		default:
			break;
		}
	}
	/*间隔一段时间查询模块状态*/
	if (mSendCommandTimeCount >= 0.1f)
	{
		snr9816tts_send_command(COMMAND_CHECK_STATE, 0, 0);
		mSendCommandTimeCount = 0;
	}
}



  void Snr9816ttsSetVoice(const char *format, ...)
 {
	va_list args;
	va_start(args, format);
	int result = vsnprintf(Snr9816data, sizeof(Snr9816data), format, args);
	va_end(args);
	snr9816tts_send_command(COMMAND_PLAY_VOICE, 0, Snr9816data);
 }

void Snr9816ttsRxCallback(UARTInstance_s *instance)
{
	Snr9816vr_t* sup = (Snr9816vr_t*)instance->superclass;
	if(instance->rx_buff[0] == 0x41)
		sup->communicate_success_flag = 1;
	else if(instance->rx_buff[0] == 0x4f)
		sup->busy_flag = 0;
	else if(instance->rx_buff[0] == 0x4e)
		sup->busy_flag = 1;
	else sup->communicate_success_flag = 0;
}

void SNR9816TTSVoicePlayInit(Snr9816vr_t *instance)
{
	snr9816tts_send_command(COMMAND_STOP_PLAY, 0, 0);
	OsDelay(50);
	snr9816tts_send_command(COMMAND_SET_VOLUME, instance->volume, 0);
	OsDelay(50);
	snr9816tts_send_command(COMMAND_SET_SPEED, instance->speed, 0);
	OsDelay(50);
	Snr9816ttsSetVoice("参数正在初始化,请耐心等待");
}
Snr9816vr_t* Snr9816ttsRegister(Snr9816vrConfig_t *config)
{
	UARTInstance_s *uart_instance =  UARTRegister(&config->uart_init);
	if (RegisterFinishFlag)
	{
		return SNR9816TTSModule;
	}
	Snr9816vr_t * instance = (Snr9816vr_t *)malloc(sizeof(Snr9816vr_t));
	if(instance == NULL){
		return NULL;
	}
	memset(instance, 0, sizeof(Snr9816vr_t));
	instance->uart_instance = uart_instance;
	instance->uart_instance->superclass = instance;
	instance->volume = 1;
	instance->speed = 1;
	SNR9816TTSVoicePlayInit(instance);
	RegisterFinishFlag = 1;
	SNR9816TTSModule = instance;
	return instance;
}