#include <stm32f10x.h>

void GPIO_clock_enable() {
    RCC->APB2ENR |= (1 << 2);  // GPIOA clock
    RCC->APB2ENR |= (1 << 4);  // GPIOC clock
}

void GPIO_Pinmode() {
    // Configure PA5 as output push-pull, 2 MHz
    GPIOA->CRL &= ~(0xF << 20);   // Clear bits for PA5 (bits 23:20)
    GPIOA->CRL |=  (0x2 << 20);   // MODE5 = 10 (Output 2 MHz), CNF5 = 00 (Push-pull)

    // Configure PC13 as input pull-up
    GPIOC->CRH &= ~(0xF << 20);   // Clear bits for PC13
    GPIOC->CRH |=  (0x8 << 20);   // CNF13=10 (input pull-up/down), MODE13=00 (input)
    GPIOC->ODR |=  (1 << 13);     // Enable pull-up on PC13
}

void delay_ms(int ms) {
    for (int i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

int main() {
    GPIO_clock_enable();
    GPIO_Pinmode();

    while (1) {
        if ((GPIOC->IDR & (1 << 13)) == 0) {  // Button pressed (active low)
            delay_ms(20);  // debounce
					 if ((GPIOC->IDR & (1 << 13)) == 0) {
            
                GPIOA->ODR = (1 << 5); 
						  
					 
					 }
					      
								
          else{

	        GPIOA->ODR = ~(1<<5);
				 

            }	
								 
        }
    }
}
