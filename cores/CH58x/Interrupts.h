#ifndef W_INTERRUPTS_CPP
#define W_INTERRUPTS_CPP

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pins_arduino.h>
#include <Arduino.h>

void (*pinIntFuncArray[32])(void);

extern "C"
__INTERRUPT
void GPIOA_IRQHandler(void) {
	uint32_t pending = (x32_INT_IF.Group & x32_INT_EN.Group) & 0x0000FFFF;
	while (pending)
	{
		int i = __builtin_ctz(pending);  // §ä³Ì§C bit

		if (pinIntFuncArray[i] != nullptr)
		{
			pinIntFuncArray[i]();
		}

		x32_INT_IF.Group = (1u << i);

		pending &= ~(1u << i); // clear bit
	}
}

extern "C"
__INTERRUPT
//__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOB_IRQHandler(void) {
	uint32_t pending = (x32_INT_IF.Group & x32_INT_EN.Group) & 0xFFFF0000;

	while (pending)
	{
		int i = __builtin_ctz(pending);   // 16~31

		if (pinIntFuncArray[i] != nullptr)
		{
			pinIntFuncArray[i]();
		}

		x32_INT_IF.Group = (1u << i);

		pending &= ~(1u << i);
	}
}

void attachInterrupt(uint8_t pin, void (*Func)(void), uint8_t Mode) {
	if (pin >= 32) return;
	uint8_t Group = pin / 16, Bit = pin % 16, G = pin / 8, B = pin % 8;
	pinIntFuncArray[pin] = Func;	
	
	switch (Mode) {
	case 3:
	{
		*pinINT_Mode_map[Group].data_reg |= (1u << Bit);
		*pinOut_map[G].data_reg |= (1u << B);
		break;
	}
	case 2:
	{
		*pinINT_Mode_map[Group].data_reg |= (1u << Bit);
		*pinOut_map[G].data_reg &= ~(1u << B);
		break;
	}
	case 1:
	{
		*pinINT_Mode_map[Group].data_reg &= ~(1u << Bit);
		*pinOut_map[G].data_reg |= (1u << B);
		break;
	}
	case 0:
	{
		*pinINT_Mode_map[Group].data_reg &= ~(1u << Bit);
		*pinOut_map[G].data_reg &= ~(1u << B);
		break;
	}
	}
	
	*pinINT_EN_map[Group].data_reg |= (1u << Bit);
	if (pin < 16) {
		PFIC_SetPriority(GPIO_A_IRQn, 0x80);
		PFIC_EnableIRQ(GPIO_A_IRQn);
	}
	else {
		PFIC_SetPriority(GPIO_B_IRQn, 0x80);
		PFIC_EnableIRQ(GPIO_B_IRQn);
	}
	
}

void detachInterrupt(uint8_t pin) {
	if (pin >= 32) return;
	uint8_t Group = pin / 16, Bit = pin % 16;

	*pinINT_EN_map[Group].data_reg &= ~(1u << Bit);
	if (x32_INT_EN.PA_EN == 0) {
		PFIC_DisableIRQ(GPIO_A_IRQn);
	}
	if (x32_INT_EN.PB_EN == 0) {
		PFIC_DisableIRQ(GPIO_B_IRQn);
	}
}
#endif
