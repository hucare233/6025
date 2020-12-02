/*
 * @Descripttion: ɧ
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-12-02 09:05:38
 * @FilePath: \6025\USER\SRC\main.c
 */
#include "main.h"
int main(void)
{
	Delay_ms(2000);
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����ϵͳ�ж����ȼ�����2
	CAN1_Configuration();
	CAN2_Mode_Init(CAN_SJW_1tq, CAN_BS2_4tq, CAN_BS1_9tq, 3, CAN_Mode_Normal); //CAN��ʼ��												   //can���г�ʼ��

	//Time_Control_Beep_Init();
	//PS2_Init();
	Led8_Configuration();
	Beep_Init();
	LED_Configuration();
	//InitCANControlList(Can2_MesgSentList, CAN_2);
	TIM3_Init();
	param_Init();
	OSInit();
	OSTaskCreate(Task_Start, (void *)0, &START_TASK_STK[START_STK_SIZE - 1], START_TASK_PRIO);
	OSStart(); //���֮��Ҫд����
}

//��ʼ����
static void Task_Start(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	Beep_Show(2);
	//play_Music_1();   //ף�����տ���
	//Play_Music(mzdhlmusic,78);
	Led8DisData(0);
	OS_CPU_SysTickInit(); //��Ҫ��������дû���������
	OS_ENTER_CRITICAL();  //�����ٽ���(�޷����жϴ��)
	OSTaskCreate(Task_Lcd, (void *)0, (OS_STK *)&LCD_TASK_STK[LCD_STK_SIZE - 1], LCD_TASK_PRIO);
	OSTaskCreate(Task_Led8, (void *)0, (OS_STK *)&LED8_TASK_STK[LED8_STK_SIZE - 1], LED8_TASK_PRIO);
	OSTaskCreate(Task_Elmo, (void *)0, (OS_STK *)&ELMO_TASK_STK[ELMO_STK_SIZE - 1], ELMO_TASK_PRIO);
	OSTaskCreate(Task_EPOS, (void *)0, (OS_STK *)&EPOS_TASK_STK[EPOS_STK_SIZE - 1], EPOS_TASK_PRIO);
	OSTaskCreate(Task_Motor, (void *)0, (OS_STK *)&MOTOR_TASK_STK[MOTOR_STK_SIZE - 1], MOTOR_TASK_PRIO);

	OSTaskSuspend(START_TASK_PRIO); //������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED0 LCD ����
static void Task_Lcd(void *pdata)
{
	while (1)
	{
		OSTimeDly(1000);
	}
}

static void Task_Led8(void *pdata) //��ˮ�ƣ����������
{
	while (1)
	{
		Led_Show();
		OSTimeDly(300);
	}
}

static void Task_Motor(void *pdata)
{
	/***************���ʹ�ܷ����������ʼ���***********/
	ENABLE_ALL_DJMOTOR_5_8
	BEGIN_ALL_DJMOTOR_5_8
	OSTimeDly(1000);
	motor[4].status.isSetZero = 1;
	motor[5].status.isSetZero = 1;
	motor[6].status.isSetZero = 1;
	motor[7].status.isSetZero = 1;
	while (1)
	{
		OSTimeDly(1100);
	}
}

static void Task_Elmo(void *pdata) //elmo����
{
	while (1)
	{
		OSTimeDly(400);
	}
}

static void Task_EPOS(void *pdata)
{
	while (1)
	{
		//		EPOS_CONTROL(5);
		//		if(EPOSflag.test)
		//		 {
		//		     PVMspeed++;
		//				 if(PVMspeed>=1000)
		//					 PVMspeed=40;
		//
		//		 /***************����**********/
		//			   EPOS_SetPVMspeed(1, PVMspeed, 1);
		//			   EPOS_SetPVMspeed(2, PVMspeed, 1);
		//			   EPOS_SetPVMspeed(3, PVMspeed, 1);
		//			   EPOS_SetPVMspeed(4, PVMspeed, 1);
		//				 EPOS_EnableOperation(1,1);
		//			   EPOS_EnableOperation(2,1);
		//			   EPOS_EnableOperation(3,1);
		//			   EPOS_EnableOperation(4,1);
		//			 }
		//		for (u8 i = 1; i < 5; i++)
		//		{
		//			EPOS_Askenable_or_disable(i, 1);
		//			EPOS_Askmode(i, 1);
		//			EPOS_Askactualspeed(i, 1);
		//			EPOS_Askdemandspeed(i, 1);
		//			EPOS_Askactualpos(i, 1);
		//			EPOS_Askdemandpos(i, 1);
		//			EPOS_Asktorque(i, 1);
		//		}
		OSTimeDly(1000);
	}
}
#ifdef USE_VESC
static void Task_VESC(void *pdata)
{
	while (1)
	{
		VESC_CONTROL(1);
		//		Can_DeQueue(CAN2, &VESC_Sendqueue); //VESC
		OSTimeDly(1000);
	}
}
#endif
#if USE_SCOPE
//ʾ��������
static void Task_Scope(void *pdata)
{
	while (1)
	{
		VS4Channal_Send(motor[1].valueReal.pulse, motor[1].valueReal.speed, 0, 10000);
		OSTimeDly(30);
	}
}
#endif

#if USE_DataScope
//��ʾ��������
static void Task_DataScope(void *pdata)
{
	u8 Send_Count;
	while (1)
	{
		DataScope_Get_Channel_Data(1.0, 1);		   //������   д��ͨ�� 1
		DataScope_Get_Channel_Data(2.0, 2);		   //������   д��ͨ�� 2
		DataScope_Get_Channel_Data(3.0, 3);		   //������   д��ͨ�� 3
		DataScope_Get_Channel_Data(400.0, 4);	   //������   д��ͨ�� 4
		DataScope_Get_Channel_Data(5.0, 5);		   //������   д��ͨ�� 5
		DataScope_Get_Channel_Data(6.0, 6);		   //������   д��ͨ�� 6
		DataScope_Get_Channel_Data(7.0, 7);		   //������   д��ͨ�� 7
		DataScope_Get_Channel_Data(8.0, 8);		   //������   д��ͨ�� 8
		DataScope_Get_Channel_Data(9.0, 9);		   //������   д��ͨ�� 9
		DataScope_Get_Channel_Data(TIM3->CNT, 10); //������   д��ͨ�� 10
		Send_Count = DataScope_Data_Generate(10);  //����ת���������м���ͨ��
		USART1_Send(DataScope_OutPut_Buffer, Send_Count);
		//		  for( u8 i = 0 ; i < Send_Count; i++)
		//          {
		//          while((USART1->SR&0X40)==0);
		//          USART1->DR = DataScope_OutPut_Buffer[i];
		//          }
		OSTimeDly(500);
	}
}
#endif
#ifdef USE_VESC
static void Task_VESCSEND(void *pdata) //TODO:���ӷ������ֹ���Ķ�ʧ
{
	while (1)
	{
		Can_DeQueue(CAN2, &VESC_Sendqueue); //VESC
		OSTimeDly(2);
	}
}
#endif