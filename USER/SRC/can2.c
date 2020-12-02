/*
 * @Descripttion: �������
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-11-29 16:29:30
 * @FilePath: \MotoPro\USER\SRC\can2.c
 */
#include "can2.h"
#include "delay.h"
#include "motor.h"

u32 timeout_ticks = 2000; //200ms
s16 timeout_counts = 0;	  //��ʱ����
u32 last_update_time[8] = {0};
u32 now_update_time[8] = {0};
u32 err_update_time[8] = {0};
bool clear_flag[8] = {0};
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;

u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

	/* Configure CAN pin: RX  TX*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE; //��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM = DISABLE; //������CAN_MCR�Ĵ�����INRQλ��1�������0��һ����⵽128������11λ������λ�����˳�����״̬
	CAN_InitStructure.CAN_AWUM = DISABLE; //˯��ģʽ����������
	CAN_InitStructure.CAN_NART = DISABLE; //��ֹ�����Զ����ͣ���ֻ����һ�Σ����۽�����
	CAN_InitStructure.CAN_RFLM = DISABLE; //���Ĳ��������µĸ��Ǿɵ�
	CAN_InitStructure.CAN_TXFP = DISABLE; //����FIFO�����ȼ��ɱ�ʶ������
	CAN_InitStructure.CAN_Mode = mode;	  //CANӲ������������ģʽ

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW = tsjw;	 //����ͬ����Ծ����Ϊһ��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1 = tbs1;	 //ʱ���1ռ��8��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2 = tbs2;	 //ʱ���2ռ��7��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler = 3; //��Ƶϵ����Fdiv��
	CAN_Init(CAN2, &CAN_InitStructure);	 //��ʼ��CAN1

	/* �����ʼ��㹫ʽ: BaudRate = APB1ʱ��Ƶ��/Fdiv/��SJW+BS1+BS2�� */
	/* 42MHz/3/(1+9+4)=1Mhz */

	//���ù�����
	//DJ
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X201 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X202 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X203 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X204 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 15;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X205 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X206 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X207 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X208 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);

	return 0;
}

static CanTxMsg DJ_tx_message;
/****DJ�����������****/
void currentInput(u8 id)
{
	PEAK(motor[id].valueSet.current, motor[id].intrinsic.CURRENT_LIMIT);
	if (!motor[id].enable)
		motor[id].valueSet.current = 0;
	if (id < 4)
		DJ_tx_message.StdId = 0x200;
	else
		DJ_tx_message.StdId = 0x1FF;
	DJ_tx_message.RTR = CAN_RTR_Data;
	DJ_tx_message.IDE = CAN_Id_Standard;
	DJ_tx_message.DLC = 8;
	u8 temp = 2 * (id & 0x0B);
	EncodeS16Data(&motor[id].valueSet.current, &DJ_tx_message.Data[temp]);
	ChangeData(&DJ_tx_message.Data[temp], &DJ_tx_message.Data[temp + 1]);
	if ((id == 3) || (id == 7))
		CAN_Transmit(CAN2, &DJ_tx_message);
}

//�жϷ�����
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	CAN_Receive(CAN2, 0, &RxMessage);
	if (
		(RxMessage.IDE == CAN_Id_Standard)												   //��׼֡��
		&& (RxMessage.IDE == CAN_RTR_Data)												   //����֡��
		&& (RxMessage.DLC == 8) && (RxMessage.StdId > 0x200) && (RxMessage.StdId < 0x209)) /* ���ݳ���Ϊ8   DJ���*/
	{
		u8 id = RxMessage.StdId - 0x201;
		last_update_time[id] = now_update_time[id];
		now_update_time[id] = OSTimeGet();
		err_update_time[id] = now_update_time[id] - last_update_time[id];
		//motor[id].valueReal.speed = (RxMessage.Data[2] << 8) | (RxMessage.Data[3]);
		motor[id].valueReal.pulseRead = (RxMessage.Data[0] << 8) | (RxMessage.Data[1]);
		motor[id].valueReal.current = (RxMessage.Data[2] << 8) | (RxMessage.Data[3]);
		motor[id].valueSet.current =(RxMessage.Data[4] << 8) | (RxMessage.Data[5]);
		motor[id].valueReal.tempeture = RxMessage.Data[6];
		//TODO:����������жϣ���Сλ�ö�ʧ
		pulse_caculate(id);
		if (!motor[id].status.clearFlag) //�ϵ��һ�ν��ж����λ�ü�����
		{
			motor[id].status.clearFlag = true;
			motor[id].argum.distance = 0;
			motor[id].valueReal.pulse = 0;
		}
	}

	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)
//len:���ݳ���(���Ϊ8)
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN2_Send_Msg(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i = 0;
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x12; // ��׼��ʶ��Ϊ0
	TxMessage.ExtId = 0x12; // ������չ��ʾ����29λ��
	TxMessage.IDE = 0;		// ʹ����չ��ʶ��
	TxMessage.RTR = 0;		// ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC = len;	// ������֡��Ϣ
	for (i = 0; i < len; i++)
		TxMessage.Data[i] = msg[i]; // ��һ֡��Ϣ
	mbox = CAN_Transmit(CAN2, &TxMessage);
	i = 0;
	while ((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
		i++; //�ȴ����ͽ���
	if (i >= 0XFFF)
		return 1;
	return 0;
}

/**
 * @author: ���˵�
 * @brief:  ��ŷ�����
 */
void valveCtrl(bool status)
{
	CanTxMsg tx_message;
	tx_message.ExtId = 0x00010400;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Extended;
	tx_message.DLC = 4;
	tx_message.Data[0] = 0x04;
	if (status)
	{
		tx_message.Data[1] = 'S' + 0x40; //��
		tx_message.Data[2] = 'N';
	}
	else
	{
		tx_message.Data[1] = 'E' + 0x40; //��
		tx_message.Data[2] = 'F';
	}
	tx_message.Data[3] = 1;
	CAN_Transmit(CAN2, &tx_message);
}