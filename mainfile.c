#include <stm32f10x.h>
#include <stdio.h>

void Uart_init(void);
void Adc_init(void);
void Uart_SendString(char *str);
void delay_ms(unsigned int ms);
void SystemClock_Config(void);

void SystemClock_Config(void)
{
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // Configure PLL: HSE * 9 = 72 MHz
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Select PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

}

void delay_ms(unsigned int ms)
{
    for (unsigned int i = 0; i < ms * 8000; i++)
        __NOP();
}

void Uart_SendString(char *str)
{
    while (*str)
    {
        USART2->DR = *str++;
        while (!(USART2->SR & USART_SR_TXE)); // Wait until TXE
    }
    while (!(USART2->SR & USART_SR_TC)); // Wait until TC
}

int main(void)
{
    SystemClock_Config();
    Uart_init();
    Adc_init();

    while (1)
    {
        ADC1->CR2 |= (1 << 22); // Start conversion

        while (!(ADC1->SR & (1 << 1))); // Wait for EOC

        uint16_t adc_val = ADC1->DR;

        float voltage = (adc_val * 3.3f) / 4096.0f;

        // Internal temp sensor voltage to temperature formula:
        float temp = ((voltage - 1.43f) / 0.0043f) + 25.0f;

        char buffer[64];
        sprintf(buffer, "Internal Temp: %.2f C\r\n", temp);
        Uart_SendString(buffer);

        delay_ms(1000);
    }
}

void Uart_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // Enable GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;  // Enable USART2

    // Configure PA2 as Alternate function push-pull
    GPIOA->CRL &= ~(0xF << 8);   // Clear CNF and MODE for PA2
    GPIOA->CRL |=  (0xB << 8);   // MODE=11 (50 MHz), CNF=10 (AF PP)

    USART2->BRR = 0x0EA6; // Baud rate for 36 MHz APB1 and 9600 baud (approx. 9600)
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE; // Enable TX and USART
}

void Adc_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= (1 << 23);  // TSVREFE = enable internal sensor
    delay_ms(1);             // >10us delay needed

    ADC1->SMPR1 |= (7 << 18);   // 239.5 cycles for channel 16
    ADC1->SQR3 = 16;            // Channel 16 (internal temp sensor)

    ADC1->CR2 |= (1 << 0);      // ADON
    for (volatile int i = 0; i < 1000; i++); // Delay

    ADC1->CR2 |= (1 << 3);      // RSTCAL
    while (ADC1->CR2 & (1 << 3));

    ADC1->CR2 |= (1 << 2);      // CAL
    while (ADC1->CR2 & (1 << 2));
}
