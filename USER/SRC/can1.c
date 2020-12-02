/*
 * @Descripttion: 主控通信
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-18 18:59:27
 * @FilePath: \MotoPro\USER\SRC\can1.c
 */
#include "can1.h"

//static s16 s16TempData[4];
//u16 u16TempData[4];

void CAN1_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  CAN_InitTypeDef CAN_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

  /* Configure CAN pin: RX A11  TX A12 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* CAN RX interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;         //非时间触发通道模式
  CAN_InitStructure.CAN_ABOM = DISABLE;         //软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
  CAN_InitStructure.CAN_AWUM = DISABLE;         //睡眠模式由软件唤醒
  CAN_InitStructure.CAN_NART = DISABLE;         //DISABLE打开报文自动发送，错误时会重发
  CAN_InitStructure.CAN_RFLM = DISABLE;         //报文不锁定，新的覆盖旧的
  CAN_InitStructure.CAN_TXFP = DISABLE;         //发送FIFO的优先级由标识符决定
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CAN硬件工作在正常模式

  /* Seting BaudRate */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度为一个时间单位
  CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; //时间段1占用8个时间单位
  CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; //时间段2占用7个时间单位
  CAN_InitStructure.CAN_Prescaler = 3;     //分频系数（Fdiv）
  CAN_Init(CAN1, &CAN_InitStructure);      //初始化CAN1

  /* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
  /* 42MHz/3/(1+9+4)=1Mhz */

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X305 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X306 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X307 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X308 << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_FilterInitStructure.CAN_FilterNumber = 3;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X320 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X301 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X302 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x303 << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_FilterInitStructure.CAN_FilterNumber = 4;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x00010500 << 8) << 3) & 0xffff0000) >> 16;
  //	CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)0x00010500 << 8) << 3) & 0xffff;
  //	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xffffff00 << 3) >> 16;
  //	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xffffff00 << 3) & 0xffff;
  CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)0x00010500 << 3) & 0xFFFF0000) >> 16;
  CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x00010500 << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
/****接收反馈原报文写入队列****/

void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg rx_message;
  if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
  {
    CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    CAN_ClearFlag(CAN1, CAN_IT_FMP0);
    CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
    /*	#ifdef VER
			if((u8)rxmsg.Data[1] >= (u8)('A'+0x40))		
			{
				CAN_Procedure.status |= BIT0;//当前报文拥有流程号
				CAN_Procedure.order_now = (u8)rxmsg.Data[rxmsg.DLC - 1];//流程号为报文最后一个字节	
				if(CAN_Procedure.status & BIT7)
				{
					CAN_Procedure.order_valid = CAN_Procedure.order_now;			
					CAN_Procedure.order_valid++;	
					CAN_Procedure.status = 0x03;				
				}
				else
				{
					CAN_Procedure.status = 0x01;							
					if(CAN_Procedure.order_valid == CAN_Procedure.order_now)
					{
						CAN_Procedure.status |= BIT1;				
						CAN_Procedure.order_valid ++;
					}
					else
					{
						CAN_Procedure.status &= ~BIT1;					
						if(CAN_Procedure.order_valid > 127)
						{
							if((CAN_Procedure.order_now >= (CAN_Procedure.order_valid - 128)) && (CAN_Procedure.order_now < CAN_Procedure.order_valid))						
								CAN_Procedure.status &= ~BIT2;
							else
								CAN_Procedure.status |= BIT2;	
						}
						else
						{
							if((CAN_Procedure.order_now <= (CAN_Procedure.order_valid + 127)) && (CAN_Procedure.order_now > CAN_Procedure.order_valid))							
								CAN_Procedure.status |= BIT2;	
							else
								CAN_Procedure.status &= ~BIT2;	
						}
					}
				}
			}
			else
			{
				CAN_Procedure.status &= ~BIT0;
			}
		#else		
			CAN_Procedure.status = (BIT0|BIT1);//流程号关闭，则默认具有正确流程号
		#endif
		*/
	}
}
