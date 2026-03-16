/*******************************************************************************
  * FILENAME    : snr9816vr.h
  * Date        : 2025/11/03 11:07:27
  * Author      : [Ma Weiqiang]
  * Version     : [Version Number]
  * Decription  : 该模块为SNR9816VR模块的接口文件，需注意使用SNR9816的通信时要把发送的频率降低，否则会出问题
 *******************************************************************************/
#ifndef SNR9816VR_H_
#define SNR9816VR_H_ 

#include "voice_def.h"
#include "bsp_uart.h"
#include "stdbool.h"

typedef  enum _SNR9816_COMMAND{
    COMMAND_SET_VOLUME = 0,  //设置音量
    COMMAND_SET_SPEED ,     //设置语速   
    COMMAND_CHECK_STATE,    //查询播放状态	
    COMMAND_STOP_PLAY,      //停止播放 	  
    COMMAND_PLAY_VOICE,     //播放声音
} Snr9816vrCommand_e;

typedef  struct snr9816vr
{
    UARTInstance_s *uart_instance;
    uint8_t volume;
    uint8_t speed;
    uint8_t communicate_success_flag;
    uint8_t busy_flag;
}Snr9816vr_t;

typedef  struct snr9816vrconfig
{
    UARTInitConfig_t uart_init;
    uint8_t volume;
    uint8_t speed;
}Snr9816vrConfig_t;


void SNR9816TTSVoicePlayInit(Snr9816vr_t *instance);
void SNR9816TTSVoicePlayMain(void);
void Snr9816ttsSetVoice(const char *format, ...);
Snr9816vr_t* Snr9816ttsRegister(Snr9816vrConfig_t *config);
#endif