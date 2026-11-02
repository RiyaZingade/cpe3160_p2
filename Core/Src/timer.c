#include "stm32l476xx.h"
#include "timer.h"
#include "dac.h"

uint16_t counter = 0;


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

// will be determining WHEN interrupts happen
// for P2 --> constant time interval
void TIMER_init(void){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // enable timer
	TIM2->PSC=0; //we want 5kHz wave with 4MHz input --- means divide by 1 so the timer runs at the full 4mHz

	// ARR is counts needed per period --> timer counts = 32000000 * 0.00005 = 1600
	TIM2->ARR = 1600-1; // 1599 since counter goes from 0 to 1599 which is 1600 counts

	// // counter restarts at 0 on each update event --> reaches 200 after 200 ticks which is 25% through period
	// TIM2->CCR1 = 1600; // 25% of 800  = 200	

	TIM2->DIER|=TIM_DIER_UIE; // update event can interrupt -- counter reached 0/rollover
	// TIM2->DIER |= TIM_DIER_CC1IE; // CCR1 match can interrupt -- counter reached 200
	NVIC_EnableIRQ(TIM2_IRQn); //CPU accepts TIM2 interrupts

	TIM2->CR1|= TIM_CR1_CEN; // start counting
}


// determines WHAT data point to output --> sends value to DAC_write()
void TIM2_IRQHandler(void){
    //UIF (update interrupt flag) --> if UIF is HIGH (1) an interrupt was given
    if(TIM2->SR & TIM_SR_UIF){ //checking if UIF bit is 1 in SR
        TIM2->SR &= ~(1 << 0);

        // use sine_table[counter] value and if counter += 1 is 8 then reset counter to 0
        DAC_write(sine_table[counter]);
        counter += 1;
        if(counter == 8){
            counter = 0;
        }


    }


}