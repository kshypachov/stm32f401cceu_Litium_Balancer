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
    // Включаем тактирование таймера TIM4
    // Таймер 4 у нас висит на шине APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // А тут настройка таймера
    // Заполняем поля структуры дефолтными значениями
    TIM_TimeBaseStructInit(&timer);
    // Выставляем предделитель
    timer.TIM_Prescaler = 520 - 1;
    // Тут значение, досчитав до которого таймер сгенерирует прерывание
    // Кстати это значение мы будем менять в самом прерывании
    timer.TIM_Period = 50;
    // Инициализируем TIM4 нашими значениями
    TIM_TimeBaseInit(TIM4, &timer);
    // Настраиваем таймер для генерации прерывания по обновлению (переполнению)
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // Запускаем таймер
    TIM_Cmd(TIM4, ENABLE);
    // Разрешаем соответствующее прерывание
    NVIC_EnableIRQ(TIM4_IRQn);
}
