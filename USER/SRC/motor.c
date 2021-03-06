/*
 * @Descripttion: DJ电机
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-02 09:05:11
 * @FilePath: \6025\USER\SRC\motor.c
 */
#include "motor.h"

MotorParam M2006instrin, M3508instrin,M6025instrin;
MotorLimit Motorlimit;
MotorArgum Motorargum;
DJmotor motor[8];
Djflag DJflag;
bool enable_or_dis;
u32 angle;
u32 Speed;
bool ifbegin;
char Motor_error[15] = "(*_ *)";

/**
 * @author: 叮咚蛋
 * @brief: 电机参数初始化
 */

void Motor_Init(void)
{
  { //电机内参
    M3508instrin.PULSE = 8192;
    M2006instrin.PULSE = 8192;
    M6025instrin.PULSE = 8192;
    M3508instrin.RATIO = 19;
    M2006instrin.RATIO = 36;
    M6025instrin.RATIO = 1;
    M3508instrin.CURRENT_LIMIT = 14745; //14745
    M2006instrin.CURRENT_LIMIT = 9000;  //1000
    M6025instrin.CURRENT_LIMIT = 2500;
    M3508instrin.GearRatio = 1;      //全局变量
    M2006instrin.GearRatio = 6.117f;    //外参齿数比
    M6025instrin.GearRatio =1;
  }
  { //电机限制保护设置
    Motorlimit.isPosLimitON = false;
    Motorlimit.maxAngle = 1800; //轮毂最多选择±0.5圈多一丢丢
    Motorlimit.isPosSPLimitOn = true;
    Motorlimit.posSPlimit = 1000;
    Motorlimit.isRealseWhenStuck = true;
    Motorlimit.zeroSP = 1000;
    Motorlimit.zeroCurrent = 2000;
    Motorlimit.stuckmotion = 1;
    Motorlimit.timeoutmotion = 0;
  }
  {                                 //电机其他参数设置
    Motorargum.timeoutTicks = 2000; //2000ms
    Motorargum.lockPulse = 0;
  }
  /****0号电机初始化****/
  motor[0].intrinsic = M2006instrin; //| M2006instrin  M3508instrin
                                     // motor[0].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[0].enable = DISABLE;
  motor[0].begin = false;
  motor[0].mode = position; //速度模式
  motor[0].valueSet.angle = 0;
  motor[0].valueSet.speed = 100;
  motor[0].valueSet.current = 50;
  PID_Init(&motor[0].PIDx, 3.5, 0.12, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[0].PIDs, 5, 0.3, 0.2, 1, motor[0].valueSet.speed);    //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[0].limit = Motorlimit;

  /****1号电机初始化****/
  motor[1].intrinsic = M2006instrin; //| M2006instrin  M3508instrin
                                     // motor[1].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[1].enable = DISABLE;
  motor[1].begin = false;
  motor[1].mode = position; //速度模式
  motor[1].valueSet.angle = 0;
  motor[1].valueSet.speed = 100;
  motor[1].valueSet.current = 50;
  PID_Init(&motor[1].PIDx, 3.5, 0.12, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[1].PIDs, 5, 0.3, 0.2, 1, motor[0].valueSet.speed);    //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[1].limit = Motorlimit;

  /****2号电机初始化****/
  motor[2].intrinsic = M2006instrin; //| M2006instrin  M3508instrin
  //motor[2].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[2].enable = DISABLE;
  motor[2].begin = false;
  motor[2].mode = position; //速度模式
  motor[2].valueSet.angle = 0;
  motor[2].valueSet.speed = 100;
  motor[2].valueSet.current = 50;
  PID_Init(&motor[2].PIDx, 3.5, 0.12, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[2].PIDs, 5, 0.3, 0.2, 1, motor[0].valueSet.speed);    //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[2].limit = Motorlimit;

  /****3号电机初始化****/
  motor[3].intrinsic = M2006instrin; //| M2006instrin  M3508instrin
  //motor[3].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[3].enable = DISABLE;
  motor[3].begin = false;
  motor[3].mode = position; //速度模式
  motor[3].valueSet.angle = 0;
  motor[3].valueSet.speed = 100;
  motor[3].valueSet.current = 50;
  PID_Init(&motor[3].PIDx, 3.5, 0.12, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[3].PIDs, 5, 0.3, 0.2, 1, motor[0].valueSet.speed);    //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[3].limit = Motorlimit;

  /****0号电机初始化****/
  motor[4].intrinsic = M6025instrin; //| M2006instrin  M3508instrin
                                     // motor[4].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[4].enable = DISABLE;
  motor[4].begin = false;
  motor[4].mode = position; //速度模式
  motor[4].valueSet.angle = -19;
  motor[4].valueSet.speed = 100;
  motor[4].valueSet.current = 50;
  PID_Init(&motor[4].PIDx, 8, 0.3, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[4].PIDs, 8, 0.3, 0, 1, motor[0].valueSet.speed);   //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[4].limit = Motorlimit;

  /****5号电机初始化****/
  motor[5].intrinsic = M6025instrin; //| M2006instrin  M3508instrin
  //motor[5].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[5].enable = DISABLE;
  motor[5].begin = false;
  motor[5].mode = position; //速度模式
  motor[5].valueSet.angle = -19;
  motor[5].valueSet.speed = 100;
  motor[5].valueSet.current = 50;
  PID_Init(&motor[5].PIDx, 8, 0.3, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[5].PIDs, 8, 0.3, 0, 1, motor[0].valueSet.speed);   //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[5].limit = Motorlimit;

  /****6号电机初始化****/
  motor[6].intrinsic = M3508instrin; //| M2006instrin  M3508instrin
                                     // motor[6].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[6].enable = DISABLE;
  motor[6].begin = false;
  motor[6].mode = position; //速度模式
  motor[6].valueSet.angle = 19;
  motor[6].valueSet.speed = 100;
  motor[6].valueSet.current = 50;
  PID_Init(&motor[6].PIDx, 8, 0.3, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[6].PIDs, 8, 0.3, 0, 1, motor[0].valueSet.speed);   //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[6].limit = Motorlimit;

  /****7号电机初始化****/
  motor[7].intrinsic = M3508instrin; //| M2006instrin  M3508instrin
                                     // motor[7].intrinsic.GearRatio=0.67f;  //可在这单独改
  motor[7].enable = DISABLE;
  motor[7].begin = false;
  motor[7].mode = position; //速度模式
  motor[7].valueSet.angle = 19;
  motor[7].valueSet.speed = 100;
  motor[7].valueSet.current = 50;
  PID_Init(&motor[7].PIDx, 8, 0.3, 0, 0.4, motor[0].valueSet.pulse); //3508 8 0.2 0 0.4    2006   3.5 0.12 0 0.4
  PID_Init(&motor[7].PIDs, 8, 0.3, 0, 1, motor[0].valueSet.speed);   //3508  8 0.3  0 1     2006   5 0.3 0.2 1
  motor[7].limit = Motorlimit;
  for (int i = 0; i < 8; i++)
  {
    motor[i].argum = Motorargum;
    motor[i].argum.maxPulse = motor[i].limit.maxAngle * motor[i].intrinsic.RATIO * motor[i].intrinsic.GearRatio * motor[i].intrinsic.PULSE / 360.f;
    motor[i].valueSet.pulse = motor[i].valueSet.angle * motor[i].intrinsic.GearRatio * motor[i].intrinsic.RATIO * motor[i].intrinsic.PULSE / 360.f;
  }
  {
    DJflag.angle = 0;
    DJflag.begin = 0;
    DJflag.enable = 0;
    DJflag.speed = 0;
    DJflag.um = 0;
  }
  for (int i = 0; i < 4; i++)
  {
    motor[i].limit.posSPlimit = 19500;
  }
}

/**
 * @author: 叮咚蛋
 * @brief: 设置当前位置为零点
 */

void setZero(DJmotor *motor)
{
  motor->status.isSetZero = false;
  motor->valueReal.pulse = 0;
  motor->valueReal.angle = 0;
}

/**
 * @author: 叮咚蛋
 * @brief: 速度模式
 */

void speed_mode(s16 id)
{
  motor[id].PIDs.SetVal = motor[id].PIDs.uKS_Coe * motor[id].valueSet.speed;
  motor[id].PIDs.CurVal = motor[id].valueReal.speed;
  PID_Operation(&motor[id].PIDs);
  motor[id].valueSet.current += motor[id].PIDs.Udlt;
}

/**
 * @author: 叮咚蛋
 * @brief: 位置模式
 */

void position_mode(s16 id)
{
  motor[id].valueSet.pulse = motor[id].valueSet.angle * motor[id].intrinsic.GearRatio * motor[id].intrinsic.RATIO * motor[id].intrinsic.PULSE / 360.f;
  motor[id].PIDx.SetVal = motor[id].valueSet.pulse;
  if (!motor[id].begin)
    motor[id].PIDx.SetVal = motor[id].argum.lockPulse; //如果为锁电机状态，位置设定屏蔽，改为锁位置
                                                       //  if (motor[id].limit.isPosLimitON)
                                                       //    PEAK(motor[id].PIDx.SetVal, motor[id].argum.maxPulse);
  motor[id].PIDx.CurVal = motor[id].valueReal.pulse;
  PID_Operation(&motor[id].PIDx);
  motor[id].PIDs.SetVal = motor[id].PIDx.uKS_Coe * motor[id].PIDx.Udlt;
  if (motor[id].limit.isPosSPLimitOn)
    PEAK(motor[id].PIDs.SetVal, motor[id].limit.posSPlimit);
  motor[id].PIDs.CurVal = motor[id].valueReal.speed;
  PID_Operation(&motor[id].PIDs);
  motor[id].valueSet.current += motor[id].PIDs.Udlt;
  if (ABS(motor[id].argum.difPulseSet) < 60)
    motor[id].status.arrived = true; //到达指定位置
  else
    motor[id].status.arrived = false;
}

/**
 * @author: 叮咚蛋
 * @brief: 寻零模式
 */

void zero_mode(s16 id) //寻零模式
{
  motor[id].PIDs.SetVal = motor[id].limit.zeroSP;
  motor[id].PIDs.CurVal = motor[id].valueReal.speed;
  PID_Operation(&motor[id].PIDs);
  motor[id].valueSet.current += motor[id].PIDs.Udlt;
  if ((motor[id].argum.distance == 0) && motor[id].valueReal.pulse > 5000)
    motor[id].argum.zeroCnt++;
  else
    motor[id].argum.zeroCnt = 0;
  if (motor[id].argum.zeroCnt > 100) //寻零完成，初始化零点，转换为位置模式锁零点
  {
    motor[id].argum.zeroCnt = 0;
    motor[id].valueReal.pulse = 0;
    motor[id].mode = position;
    motor[id].status.zero = 1;
    motor[id].valueSet.angle = 0;
  }
}

/**
 * @author: 叮咚蛋
 * @brief: 位置计算
 */

void pulse_caculate(u8 id)
{

    motor[id].argum.distance = motor[id].valueReal.pulseRead - motor[id].valuePrv.pulseRead;
    motor[id].valuePrv = motor[id].valueReal;
    if (ABS(motor[id].argum.distance) > 4150) //last edit : 4085 ;
      motor[id].argum.distance -= SIG(motor[id].argum.distance) * motor[id].intrinsic.PULSE;
    motor[id].valueReal.pulse += motor[id].argum.distance;                              //累计脉冲计算
    motor[id].argum.difPulseSet = motor[id].valueSet.pulse - motor[id].valueReal.pulse; //更新误差
    motor[id].valueReal.angle = motor[id].valueReal.pulse * 360.f / motor[id].intrinsic.RATIO / motor[id].intrinsic.GearRatio / motor[id].intrinsic.PULSE;

    if (motor[id].begin)
      motor[id].argum.lockPulse = motor[id].valueReal.pulse;
    /* 判断是否需要重置零点 */
    if (motor[id].status.isSetZero)
      setZero(&motor[id]);

}


void Set_Gimbal_Current(s16 gimbal_yaw_iq, s16 gimbal_pitch_iq)
{
CanTxMsg tx_message;   
tx_message.StdId = 0x1ff;//2016的官方开源给的对应2016的6025电调板，地址0x1FF
tx_message.IDE = CAN_Id_Standard;
tx_message.RTR = CAN_RTR_Data;
tx_message.DLC = 0x08;

tx_message.Data[0] = (unsigned char)(gimbal_pitch_iq >> 8);
tx_message.Data[1] = (unsigned char)gimbal_pitch_iq;

tx_message.Data[2] = 0x00;
tx_message.Data[3] = 0x00;

tx_message.Data[4] = (unsigned char)(gimbal_yaw_iq >> 8);
tx_message.Data[5] = (unsigned char)gimbal_yaw_iq;
tx_message.Data[6] = 0x00;
tx_message.Data[7] = 0x00;

CAN_Transmit(CAN2,&tx_message);
}