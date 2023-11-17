/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2022-09-14

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "definitions.h"
#include "stm32f4xx.h"
//--------STM32Lib-------------------------------------------//
/*
#include "system_stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_crc.h"
#include "stdbool.h"
*/
#ifndef DEBUG_APP
	#include "misc.h"
#endif
//---------------------STM32Lib------------------------------//
//--------FreeRTOS Lib---------------------------------------//
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

//#ifdef FreeRTOS_DEBUG
//	#include "FreeRTOS_debug.h"
//#endif
//--------FreeRTOS Lib---------------------------------------//

/* Private macro */
#define Li_ion_4_20		4.20
#define Li_ion_4_15		4.15
#define Li_ion_4_10		4.10
#define Li_ion_4_05		4.05
#define Li_ion_4_00		4.00
#define Li_ion_3_95		3.95
#define Li_ion_3_90		3.90
#define Li_ion_3_85		3.85
#define Li_ion_3_80		3.80
#define Li_ion_3_75		3.75


#define LiFePO4_3_65	3.65
#define LiFePO4_3_60	3.60
#define LiFePO4_3_55	3.55
#define LiFePO4_3_50	3.50
#define LiFePO4_3_45	3.45
#define	LiFePO4_3_40	3.40
#define LiFePO4_3_35	3.35

#define SW0				0b1111
#define SW1				0b0111
#define SW2				0b1011
#define SW3				0b0011
#define SW4				0b1101
#define SW5				0b0101
#define SW6				0b1001
#define SW7				0b0001

#define SW8				0b1110
#define SW9				0b0110
#define SW10			0b1010
#define SW11			0b0010
#define SW12			0b1100
#define SW13			0b0100
#define SW14			0b1000
#define SW15			0b0000

#define delay_0_5s		50
#define delay_1s		100
#define delay_5s		500
#define delay_2s		200
#define delay_10s		1000
/* Private variables */

xQueueHandle    configQ;
xQueueHandle    battVoltQ;
/* Private function prototypes */
/* Private functions */

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {

    ( void ) pcTaskName;
    ( void ) pxTask;

	taskDISABLE_INTERRUPTS(); // game over

  for( ;; );
}

void vApplicationMallocFailedHook(){

	taskDISABLE_INTERRUPTS(); // game over
	for( ;; );

}

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

#define ADC_CH			2;
#define ADC_SampleTime	ADC_SampleTime_112Cycles
#define BuffSize		2
//uint32_t	AdcBuf[BuffSize];
unsigned short  AdcBuf[BuffSize]; //AdcBuf[0] - battAdc, AdcBuf[1] - Vref
// PB1 ADC1_IN9

#define DeadTime 1
#define Period	 1000
#define MaxChPWM 4


void ConfigTimer(void){

    TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef timerPWM;
    TIM_BDTRInitTypeDef timerBDTR;

    TIM_BDTRStructInit(&timerBDTR);
    timerBDTR.TIM_DeadTime = DeadTime;
    TIM_BDTRConfig(TIM1,&timerBDTR);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler 	= 0;
    timer.TIM_Period 		= Period;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode 	= TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &timer);

    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_OCMode         = TIM_OCMode_PWM2;
    timerPWM.TIM_OutputState    = TIM_OutputState_Enable;
    timerPWM.TIM_OutputNState	= TIM_OutputNState_Enable;
    timerPWM.TIM_Pulse 			= 0;
    timerPWM.TIM_OCPolarity     = TIM_OCPolarity_Low;
    timerPWM.TIM_OCNPolarity	= TIM_OCNPolarity_Low;
    TIM_OC1Init(TIM1, &timerPWM);

    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_OCMode         = TIM_OCMode_PWM2;
    timerPWM.TIM_OutputState    = TIM_OutputState_Enable;
    timerPWM.TIM_OutputNState	= TIM_OutputNState_Enable;
    timerPWM.TIM_Pulse 			= 0;
    timerPWM.TIM_OCPolarity     = TIM_OCPolarity_Low;
    timerPWM.TIM_OCNPolarity	= TIM_OCNPolarity_Low;
    TIM_OC2Init(TIM1, &timerPWM);

    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_OCMode         = TIM_OCMode_PWM2;
    timerPWM.TIM_OutputState    = TIM_OutputState_Enable;
    timerPWM.TIM_OutputNState	= TIM_OutputNState_Enable;
    timerPWM.TIM_Pulse 			= 0;
    timerPWM.TIM_OCPolarity     = TIM_OCPolarity_Low;
    timerPWM.TIM_OCNPolarity	= TIM_OCNPolarity_Low;
    TIM_OC3Init(TIM1, &timerPWM);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    TIM_Cmd(TIM1, ENABLE);

//---------TIM8-----------------------------------

    TIM_BDTRStructInit(&timerBDTR);
    timerBDTR.TIM_DeadTime = DeadTime;
    TIM_BDTRConfig(TIM8,&timerBDTR);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler 	= 0;
    timer.TIM_Period 		= Period;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode 	= TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &timer);

    TIM_OCStructInit(&timerPWM);
    timerPWM.TIM_OCMode         = TIM_OCMode_PWM2;
    timerPWM.TIM_OutputState    = TIM_OutputState_Enable;
    timerPWM.TIM_OutputNState	= TIM_OutputNState_Enable;
    timerPWM.TIM_Pulse 			= 0;
    timerPWM.TIM_OCPolarity     = TIM_OCPolarity_Low;
    timerPWM.TIM_OCNPolarity	= TIM_OCNPolarity_Low;
    TIM_OC1Init(TIM8, &timerPWM);

    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);

    //-----------TEST----------------
    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
    //-----------TEST----------------

    TIM_Cmd(TIM8, ENABLE);
}

void ADC_Config(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;

	/* DMA2 Stream0 channel2 configuration **************************************/
	DMA_Cmd(DMA2_Stream0, DISABLE);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&ADC1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (unsigned int)&AdcBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = BuffSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = 0;

	//--------------------TEST-------------------------
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T8_TRGO;

	//--------------------TEST-------------------------

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = ADC_CH;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel configuration **************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 2, ADC_SampleTime);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

//	enable continius conv mode
//	ADC_ContinuousModeCmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);


	/* Start ADC1 */
	ADC_SoftwareStartConv(ADC1);

	ADC_TempSensorVrefintCmd(ENABLE);
}

void GPIO_Config(void){
	GPIO_InitTypeDef    GPIO_InitStructure;
	/* Configure analog sensors pins ****************/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure DIP switch pins
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure resistor switchon pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure resistors division pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void vGetConfig (void *pvParameters){

	uint16_t port;
	float balanceVoltage = 10; //��� ���� �������� ������������ �� �������� �������

	while(1){
		port = GPIO_ReadInputData(GPIOA);
		port = port & 0b1111;

		switch (port) {
			case SW0:
				balanceVoltage = 10;//�������� �� ������, ������������ �� ��������
				break;
			case SW1:
				balanceVoltage = LiFePO4_3_65;
				break;
			case SW2:
				balanceVoltage = LiFePO4_3_60;
				break;
			case SW3:
				balanceVoltage = LiFePO4_3_55;
				break;
			case SW4:
				balanceVoltage = LiFePO4_3_50;
				break;
			case SW5:
				balanceVoltage = LiFePO4_3_45;
				break;
			case SW6:
				balanceVoltage = LiFePO4_3_40;
				break;
			case SW7:
				balanceVoltage = LiFePO4_3_35;
				break;
			case SW8:
				balanceVoltage = Li_ion_4_20;
				break;
			case SW9:
				balanceVoltage = Li_ion_4_15;
				break;
			case SW10:
				balanceVoltage = Li_ion_4_10;
				break;
			case SW11:
				balanceVoltage = Li_ion_4_05;
				break;
			case SW12:
				balanceVoltage = Li_ion_4_00;
				break;
			case SW13:
				balanceVoltage = Li_ion_3_95;
				break;
			case SW14:
				balanceVoltage = Li_ion_3_90;
				break;
			case SW15:
				balanceVoltage = Li_ion_3_85;
				break;
			default:
				balanceVoltage = 10; //������� �� �������� ������������
				break;
		}
		xQueueOverwrite(configQ, (void *) &balanceVoltage);
		vTaskDelay(delay_10s); // 10 ������ �����
	}
	vTaskDelete(NULL);
}


void vGetBattVoltage (void *pvParameters){

	float 		battVoltage;
	uint16_t	Vdd;

	//uint16_t	Vref;
	//Vref = 3300*(*VREFINT_CAL)/4095;

	while(1){
		vTaskDelay(delay_1s);
		//GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		ADC_SoftwareStartConv(ADC1);
		vTaskDelay(delay_1s);
		//GPIO_SetBits(GPIOA, GPIO_Pin_8);

		Vdd 		= 3300*(*VREFINT_CAL)/AdcBuf[1];
		battVoltage = Vdd * AdcBuf[0] / 4095;
		battVoltage = battVoltage / (resDivCoef + 0.0001);
		battVoltage = battVoltage / 1000;
		battVoltage = battVoltage;

		xQueueOverwrite(battVoltQ, (void *) &battVoltage);
	}
	vTaskDelete(NULL);
}

void vMainMgmt (void *pvParameters){

	float 		battVoltage;
	float 		configBalanceVoltage;

	vTaskDelay(delay_5s);

	while(1){
		xQueuePeek(configQ,		&configBalanceVoltage,	0);
		xQueuePeek(battVoltQ,	&battVoltage,			0);

		if(battVoltage > configBalanceVoltage){
			//Switchon balance resistor
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);
			vTaskDelay(delay_1s);
		}else{
			//Switchoff balance resistor
			GPIO_SetBits(GPIOB, GPIO_Pin_15);
			vTaskDelay(delay_1s);
		}
	}
	vTaskDelete(NULL);
}

void vWatchdogTask (void *pvParameters){

	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);
	/* Wait till LSI is ready */

	int 	sw = 0;

	while (sw){
		vTaskDelay(delay_5s);
	}

	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
		vTaskDelay(delay_0_5s);
	}

	/* Enable Watchdog*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64); // 4, 8, 16 ... 256
	IWDG_SetReload(0x0900);//This parameter must be a number between 0 and 0x0FFF.
	IWDG_ReloadCounter();
	IWDG_Enable();
	while (1)
	{
		IWDG_ReloadCounter();
		vTaskDelay(delay_2s);
	}
	vTaskDelete(NULL);
}

int main(void)
{

//Config GPIO RCC ************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//Config GPIO RCC ------------------------------------

//Config ADC RCC ************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//Config ADC RCC -------------------------------------

//Config DMA RCC ************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//Config DMA RCC -------------------------------------

	GPIO_Config();
	ADC_Config();
//	ConfigTimer();

	configQ 	= xQueueCreate( QUEUE_LENGTH, sizeof(float));
	battVoltQ	= xQueueCreate( QUEUE_LENGTH, sizeof(float));

  /* TODO - Add your application code here */

	xTaskCreate (vGetBattVoltage ,(signed char*)"vGetBattVoltage"	, 200, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate (vGetConfig 	 ,(signed char*)"vGetConfig"		, 200, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate (vMainMgmt 		 ,(signed char*)"vMainMgmt"			, 200, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate (vWatchdogTask	 ,(signed char*)"vWatchdogTask"		, 200, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

}


