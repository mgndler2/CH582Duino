#pragma once

#ifndef __CH58xGPIOSFR__
#define __CH58xGPIOSFR__
#include "CH58x_SFR_TOP_LVL.h" 
//0x18
typedef union PIN_ALTERNATE {
	 struct {
		  uint16_t PIN_TMR0 : 1;
		  uint16_t PIN_TMR1 : 1;
		  uint16_t PIN_TMR2 : 1;
		  uint16_t PIN_TMR3 : 1;
		  uint16_t PIN_UART0 : 1;
		  uint16_t PIN_UART1 : 1;
		  uint16_t PIN_UART2 : 1;
		  uint16_t PIN_UART3 : 1;

		  uint16_t PIN_SPI0 : 1;
		  uint16_t : 1;
		  uint16_t PIN_PWMX : 1;
		  uint16_t PIN_I2C : 1;
		  uint16_t PIN_MODEM : 1;
		  uint16_t PIN_INTX : 1;
		  uint16_t PIN_U0_INV : 1;
		  uint16_t PIN_RF_ANT_SW_EN : 1;
	 };
	 uint16_t raw;
}PIN_ALTERNATE_t;
//0x1A
typedef union PIN_ANALOG_IE {
	 struct {
		  uint16_t PIN_ADC8_9_IE : 1;
		  uint16_t PIN_ADC6_7_IE : 1;
		  uint16_t PIN_ADC10_IE : 1;
		  uint16_t PIN_ADC11_IE : 1;
		  uint16_t PIN_USB2_DP_PU : 1;
		  uint16_t PIN_USB2_IE : 1;
		  uint16_t PIN_USB_DP_PU : 1;
		  uint16_t PIN_USB_IE : 1;
		  		  
		  uint16_t : 1;
		  uint16_t PIN_ADC0_IE : 1;
		  uint16_t PIN_ADC1_IE : 1;
		  uint16_t PIN_ADC12_IE : 1;
		  uint16_t PIN_ADC13_IE : 1;
		  uint16_t PIN_XT32K_IE : 1;
		  uint16_t PIN_ADC2_3_IE : 1;
		  uint16_t PIN_ADC4_5_IE : 1;
	 };
	 uint16_t raw;
}PIN_ANALOG_IE_t;
//0x90
typedef union INT_EN {
	 struct {
		 uint16_t PA_EN;
		 uint16_t PB_EN;
	 };
	 uint32_t Group;
}INT_EN_t;
//0x94
typedef union INT_MODE {
	 struct {
		 uint16_t PA_MODE;
		 uint16_t PB_MODE;
	 };
	 uint32_t Group;
}INT_MODE_t;
//0x9C
typedef union INT_IF {
	 struct {
		 uint16_t PA_IF;
		 uint16_t PB_IF;
	 };
	 uint32_t Group;
}INT_IF_t;
//0xA0
typedef union PA_DIR {
	 struct {
		  uint8_t PA_DIR_0;
		  uint8_t PA_DIR_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_DIR_t;
//0xA4
typedef union PA_PIN {
	 struct {
		  uint8_t PA_PIN_0;
		  uint8_t PA_PIN_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_PIN_t;
//0xA8
typedef union PA_OUT {
	 struct {
		  uint8_t PA_OUT_0;
		  uint8_t PA_OUT_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_OUT_t;
//0xAC
typedef union PA_CLR {
	 struct {
		  uint8_t PA_CLR_0;
		  uint8_t PA_CLR_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_CLR_t;
//0xB0
typedef union PA_PU {
	 struct {
		  uint8_t PA_PU_0;
		  uint8_t PA_PU_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_PU_t;
//0xB4
typedef union PA_PD_DRV {
	 struct {
		  uint8_t PA_PD_DRV_0;
		  uint8_t PA_PD_DRV_1;
		  uint16_t : 16;
	 };
	 uint32_t Group;
}PA_PD_DRV_t;
//0xC0
typedef union PB_DIR {
	 struct {
		  uint8_t PB_DIR_0;
		  uint8_t PB_DIR_1;
		  uint8_t PB_DIR_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_DIR_t;
//0xC4
typedef union PB_PIN {
	 struct {
		  uint8_t PB_PIN_0;
		  uint8_t PB_PIN_1;
		  uint8_t PB_PIN_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_PIN_t;
//0xC8
typedef union PB_OUT {
	 struct {
		  uint8_t PB_OUT_0;
		  uint8_t PB_OUT_1;
		  uint8_t PB_OUT_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_OUT_t;
//0xCC
typedef union PB_CLR {
	 struct {
		  uint8_t PB_CLR_0;
		  uint8_t PB_CLR_1;
		  uint8_t PB_CLR_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_CLR_t;
//0xD0
typedef union PB_PU {
	 struct {
		  uint8_t PB_PU_0;
		  uint8_t PB_PU_1;
		  uint8_t PB_PU_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_PU_t;
//0xD4
typedef union PB_PD_DRV {
	 struct {
		  uint8_t PB_PD_DRV_0;
		  uint8_t PB_PD_DRV_1;
		  uint8_t PB_PD_DRV_2;
		  uint8_t : 8;
	 };
	 uint32_t Group;
}PB_PD_DRV_t;

typedef union ALTERNATIVE {
	 struct {
		  PIN_ALTERNATE_t	PIN_ALTERNATE;	 //0x18-19
		  PIN_ANALOG_IE_t	PIN_ANALOG_IE;	 //0x1A-1B//
	 };
}ALTERNATIVE_t;

typedef union PIN_INT {
	struct {
		INT_EN_t		INT_EN;		//0x90-93//
		INT_MODE_t		INT_MODE;	//0x94-97//
		uint32_t : 32;				//0x98-9B//
		INT_IF_t		INT_IF;		//0x9C-9F//
	};
}PIN_INT_t;

typedef union PIN_A_GROUP {
	struct {
		PA_DIR_t	PA_DIR;		//A0
		PA_PIN_t	PA_PIN;		//A4
		PA_OUT_t	PA_OUT;		//A8
		PA_CLR_t	PA_CLR;		//AC
		PA_PU_t		PA_PU;		//B0
		PA_PD_DRV_t	PA_PD_DRV;  //B4
	};
}PIN_A_GROUP_t;

typedef union PIN_B_GROUP {
	struct {
		PB_DIR_t	PB_DIR;		//C0
		PB_PIN_t	PB_PIN;		//C4
		PB_OUT_t	PB_OUT;		//C8
		PB_CLR_t	PB_CLR;		//CC
		PB_PU_t		PB_PU;		//D0
		PB_PD_DRV_t	PB_PD_DRV;  //D4
	};
}PIN_B_GROUP_t;
//
// ALTERNATE
//
#define ALTERNATE		(*(volatile ALTERNATIVE_t*)		0x40001018)
#define x16_PIN_ALTERNATE	(*(volatile PIN_ALTERNATE_t*)	0x40001018)
#define x16_PIN_ANALOG_IE	(*(volatile PIN_ANALOG_IE_t*)	0x4000101A)
//
// PIN_INT
//
#define PIN_INT			(*(volatile PIN_INT_t*)			0x40001090)
#define x32_INT_EN			(*(volatile INT_EN_t*)			0x40001090)
#define x32_INT_MODE		(*(volatile INT_MODE_t*)		0x40001094)
/*-----0x40001098-9B is RSVD-----*/
#define x32_INT_IF			(*(volatile INT_IF_t*)			0x4000109C)
//
// PIN_A
//
#define PIN_A			(*(volatile PIN_A_GROUP_t*)		0x400010A0)
#define x32_PA_DIR			(*(volatile PA_DIR_t*)			0x400010A0)
#define x32_PA_PIN			(*(volatile PA_PIN_t*)			0x400010A4)
#define x32_PA_OUT			(*(volatile PA_OUT_t*)			0x400010A8)
#define x32_PA_CLR			(*(volatile PA_CLR_t*)			0x400010AC)
#define x32_PA_PU			(*(volatile PA_PU_t*)			0x400010B0)
#define x32_PA_PD_DRV		(*(volatile PA_PD_DRV_t*)		0x400010B4)
//
// PIN_B
//
#define PIN_B			(*(volatile PIN_B_GROUP_t*)		0x400010C0)
#define x32_PB_DIR			(*(volatile PB_DIR_t*)			0x400010C0)
#define x32_PB_PIN			(*(volatile PB_PIN_t*)			0x400010C4)
#define x32_PB_OUT			(*(volatile PB_OUT_t*)			0x400010C8)
#define x32_PB_CLR			(*(volatile PB_CLR_t*)			0x400010CC)
#define x32_PB_PU			(*(volatile PB_PU_t*)			0x400010D0)
#define x32_PB_PD_DRV		(*(volatile PB_PD_DRV_t*)		0x400010D4)
#endif // !__CH58xGPIOSFR__


