#include "stm32f10x.h"

                                                 // Initialize PWM on PA0 using TIM2 Channel 1
void TIM2_PWM_Init(void) {
    // 1. Enable clocks for GPIOA and TIM2
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;        // Enable GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;       // Enable TIM2 clock

                                             // 2. Set PA0 (TIM2_CH1) to alternate function push-pull
    GPIOA->CRL &= ~GPIO_CRL_CNF0;           // Clear CNF
    GPIOA->CRL |= GPIO_CRL_CNF0_1;          // CNF0[1:0] = 10 (AF push-pull)
    GPIOA->CRL |= GPIO_CRL_MODE0_1;         // MODE0[1:0] = 01 (Output mode, 2 MHz)

                                         // 3. Set up TIM2 for PWM
    TIM2->PSC = 72 - 1;                 // Prescaler: 72MHz / 72 = 1 MHz timer clock
    TIM2->ARR = 20000 - 1;             // Auto-reload value: 20ms (50Hz PWM)

    TIM2->CCMR1 |= (6 << 4);          // PWM mode 1 on channel 1 (OC1M = 110)
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;   // Output compare preload enable
    TIM2->CCER |= TIM_CCER_CC1E;      // Enable channel 1 output
    TIM2->CR1 |= TIM_CR1_ARPE;        // Enable auto-reload preload
    TIM2->CR1 |= TIM_CR1_CEN;         // Enable timer

    TIM2->CCR1 = 1500;  // Default pulse width (1.5ms) — center position
}

// Delay function using busy wait (not accurate, good for basic testing)
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

int main(void) {
	
	
    TIM2_PWM_Init();

    while (1) {
			
			
			for(int i=500; i<1500; i+=10){
        
       TIM2->CCR1 = i;
      delay_ms(5);}
			
			for(int i=1500; i>500; i-=10){

       TIM2->CCR1 = i;
      delay_ms(5);}
     
    }
}
