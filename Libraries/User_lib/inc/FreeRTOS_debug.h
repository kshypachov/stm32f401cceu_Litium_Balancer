#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

volatile unsigned long ulHighFrequencyTimerTicks;

void TIM7_IRQHandler(void);
void SetupRunTimeStatsTimer(void);

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()   SetupRunTimeStatsTimer()
#define portGET_RUN_TIME_COUNTER_VALUE()           ulHighFrequencyTimerTicks
#define configUSE_TRACE_FACILITY                   1
#define configGENERATE_RUN_TIME_STATS              1
#define configUSE_STATS_FORMATTING_FUNCTIONS    	1
