#ifndef _VOFA_H__
#define _VOFA_H__
#include "stdint.h"
/**
 *  @brief vofa初始化
 */
void VOFA_Init(void);
/**
 * @brief   vofa发送4个浮点数
 * @param  data1~data4 需要发送的浮点数
 */
void VOFA_SEND_4(float data1, float data2, float data3, float data4);
/**
 * @brief   vofa发送8个浮点数
 * @param  data1~data8 需要发送的浮点数
 */
void VOFA_SEND_8(float data1, float data2, float data3, float data4,
                 float data5, float data6, float data7, float data8);
/**
 * @brief   vofa发送16个浮点数
 * @param  data1~data16 需要发送的浮点数
 */
void VOFA_SEND_16(float data1, float data2, float data3, float  data4,
                 float data5, float data6, float data7, float data8,
                 float data9, float data10, float data11, float data12,
                 float data13, float data14, float data15, float data16);
#endif
