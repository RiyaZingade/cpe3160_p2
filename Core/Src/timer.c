#include "stm32l476xx.h"
#include "timer.h"



void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


void timer_init(void){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // enable timer
	TIM2->PSC=0; //we want 5kHz wave with 4MHz input --- means divide by 1 so the timer runs at the full 4mHz

	// ARR is counts needed per period = 4000000/5000 = 800
	TIM2->ARR = 6400-1; // 799 since counter goes from 0 to 799 which is 800 counts

	// counter restarts at 0 on each update event --> reaches 200 after 200 ticks which is 25% through period
	TIM2->CCR1 = 1600; // 25% of 800  = 200	

	TIM2->DIER|=TIM_DIER_UIE; // update event can interrupt -- counter reached 0/rollover
	TIM2->DIER |= TIM_DIER_CC1IE; // CCR1 match can interrupt -- counter reached 200
	NVIC_EnableIRQ(TIM2_IRQn); //CPU accepts TIM2 interrupts

	TIM2->CR1|= TIM_CR1_CEN; // start counting
}

void TIM2_IRQHandler(void){
	

	//if CC1IF is 1 then that means interrupt happened because of counter reaching 200 -- this is when we turn PA5 to LOW
	if(TIM2->SR & TIM_SR_CC1IF){
		// turning PA5 to LOW
		GPIOA->ODR &= ~(1 << TIMER_PIN);
		TIM2->SR &= ~(TIM_SR_CC1IF);
	}
	
	if(TIM2->SR & TIM_SR_UIF){
		GPIOA->ODR |= (1 << TIMER_PIN);
		TIM2->SR &= ~(TIM_SR_UIF);
	}

}