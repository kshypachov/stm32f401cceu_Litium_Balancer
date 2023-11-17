//#include "FreeRTOS_debug.h"


#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "FreeRTOSConfig.h"


void TIM4_IRQHandler()
{

        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        ulHighFrequencyTimerTicks++;


}

void SetupRunTimeStatsTimer(void)
{
	TIM_TimeBaseInitTypeDef timer;
    // �������� ������������ ������� TIM4
    // ������ 4 � ��� ����� �� ���� APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // � ��� ��������� �������
    // ��������� ���� ��������� ���������� ����������
    TIM_TimeBaseStructInit(&timer);
    // ���������� ������������
    timer.TIM_Prescaler = 520 - 1;
    // ��� ��������, �������� �� �������� ������ ����������� ����������
    // ������ ��� �������� �� ����� ������ � ����� ����������
    timer.TIM_Period = 50;
    // �������������� TIM4 ������ ����������
    TIM_TimeBaseInit(TIM4, &timer);
    // ����������� ������ ��� ��������� ���������� �� ���������� (������������)
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // ��������� ������
    TIM_Cmd(TIM4, ENABLE);
    // ��������� ��������������� ����������
    NVIC_EnableIRQ(TIM4_IRQn);
}
