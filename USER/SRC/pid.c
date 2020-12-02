/*
 * @Descripttion: pid¿ØÖÆ
 * @version: µÚ¶þ°æ
 * @Author: ¶£ßËµ°
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: ¶£ßËµ°
 * @LastEditTime: 2020-11-06 20:08:18
 * @FilePath: \MotoPro\USER\SRC\pid.c
 */
#include "pid.h"
#include "can2.h"

/**
 * @author: ¶£ßËµ°
 * @brief: pid³õÊ¼»¯
 * @param PID_setTypeDef
 */

void PID_Init(PID_setTypeDef *PID, float KP, float KI, float KD, float KS, s32 Set)
{
	PID->CurVal = 0;
	PID->SetVal = Set;
	PID->liEkVal[0] = 0;
	PID->liEkVal[1] = 0;
	PID->liEkVal[2] = 0;
	PID->uKD_Coe = KD;
	PID->uKP_Coe = KP;
	PID->uKI_Coe = KI;
	PID->uKS_Coe = KS;
}

/**
 * @author: ¶£ßËµ°
 * @brief: ½øÐÐpidÔËËã
 * @param PID_setTypeDef
 */

void PID_Operation(PID_setTypeDef *PID)
{
	PID->liEkVal[0] = PID->SetVal - PID->CurVal;
	PID->Udlt = PID->uKP_Coe * (PID->liEkVal[0] - PID->liEkVal[1]) + PID->uKI_Coe * PID->liEkVal[0] + PID->uKD_Coe * (PID->liEkVal[0] + PID->liEkVal[2] - PID->liEkVal[1] * 2);
	PID->liEkVal[2] = PID->liEkVal[1];
	PID->liEkVal[1] = PID->liEkVal[0];
}

void peakcurrent(void)
{
	for (u8 id = 0; id < 8; id++)
		PEAK(motor[id].valueSet.current, motor[id].intrinsic.CURRENT_LIMIT);
}
