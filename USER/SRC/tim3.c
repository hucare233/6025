/*
 * @Descripttion: ��ʱ���������ķ���
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-11-29 20:32:34
 * @FilePath: \MotoPro\USER\SRC\tim3.c
 */
#include "tim3.h"
#include "pid.h"
#include "can2.h"
#include "motor.h"

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); ///ʹ��TIM3ʱ��

    TIM_TimeBaseInitStructure.TIM_Period = 11999;                   //�Զ���װ��ֵ11999
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7;                    //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //��ʼ��TIM3

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //����ʱ��3�����ж�
    TIM_Cmd(TIM3, ENABLE);                     //ʹ�ܶ�ʱ��3

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           //��ʱ��3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ
        //pulse_caculate();
        for (int id = 0; id < 8; id++)
        {
            if (motor[id].enable)
            {
                if (motor[id].begin)
                {
                    switch (motor[id].mode)
                    {
                    case current:;
                        break;
                    case RPM:
                        speed_mode(id);
                        break; //�ٶ�ģʽ
                    case position:
                        position_mode(id);
                        break; //λ��ģʽ
                    case zero:
                        zero_mode(id);
                        break; //Ѱ��ģʽ
                    default:
                        break;
                    }
                }
                else
                    position_mode(id);
            }
            //currentInput(id);
        }
        peakcurrent(); //DJ��������

    }
}
