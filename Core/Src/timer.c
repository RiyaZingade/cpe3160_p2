#include "stm32l476xx.h"
#include "timer.h"
#include "dac.h"

#define SINE_POINTS 200

uint16_t counter = 0;
static uint16_t sine_table[SINE_POINTS];

waveform_type curr_waveform = WAVEFORM_SQUARE; // square waveform required at startup


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

void sine_table_generator(){
    for (uint16_t i = 0; i < SINE_POINTS; i++) {

        double angle = (2.0 * 3.141592653589793 * i) / SINE_POINTS;

        double voltage = 1.5 + 1.5 * sin(angle);

        // TODO: Convert voltage into a 12-bit DAC code
        uint16_t code = voltage/3.3 * 4096;
        // TODO: Store the result in sine_table[i]
        sine_table[i] = code;
    }
}

uint16_t waveform_generator(waveform_type type, uint16_t count, float duty_cycle){

    uint16_t code = 0;

    switch(type){
        case WAVEFORM_SQUARE:
            int first_half_range = 200 * duty_cycle; // this will be from 0 - low_range and the second part will be low_range + 1 to 199
            if(count < first_half_range){
                code = DAC_HIGH_CODE;
            } else {
                code = DAC_LOW_CODE;
            }
            //logic
            break;
        case WAVEFORM_TRIANGLE:
            float slope = 37.24; //--> 3724 is the 3.0 voltage value in decimal format (practiced for midterm2) since DAC only takes digital values
                                // whole point of the DIGITAL to ANALOG part
            if(count < 100){
                code = slope * count;
            } else {
                code = 3724*(200-count)/100; //7448 is double 3724 which we got by figuring out at count 100 we want code to be 3724
            }

            
            // logic
            break;
        case WAVEFORM_SAWTOOTH:
            code = (3724UL * count)/200U;
            break;
        case WAVEFORM_SINE:
            code = sine_table[count];
            break;
    }

    return code;

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
        // DAC_write(sine_table[counter]);
        counter += 1;
        if(counter == 200){
            counter = 0;
        }
        //depending on buttons clicked, change this line below
        DAC_write(waveform_generator(curr_waveform, counter, 0));


    }


}