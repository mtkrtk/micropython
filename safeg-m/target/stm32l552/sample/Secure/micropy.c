#include "micropy.h"
#include "kernel_cfg.h"
#include "stm32l552xx.h"

void initializer(intptr_t exinf)
{
	GPIOC->SECCFGR = 1 << 7; /* Set LED1 secure */
	GPIOC->MODER = (GPIOC->MODER & 0b11 << 14) | (0b01 << 14); /* Set LED1 output */
	GPIOC->OTYPER &= ~(1 << 7); /* Set LED1 push-pull */
}

void task(intptr_t exinf)
{
	GPIOC->ODR ^= 1 << 7; /* Toggle LED1 */
}
