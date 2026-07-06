#pragma once

#ifndef __CH58xPMUSFR__
#define __CH58xPMUSFR__
#include "CH58x_SFR_TOP_LVL.h" 

typedef union SLP_CLK_OFF {
	struct {
		uint8_t CLK_TMR0 : 1;
		uint8_t CLK_TMR1 : 1;
		uint8_t CLK_TMR2 : 1;
		uint8_t CLK_TMR3 : 1;
		uint8_t CLK_UART0 : 1;
		uint8_t CLK_UART1 : 1;
		uint8_t CLK_UART2 : 1;
		uint8_t CLK_UART3 : 1;

		uint8_t CLK_SPI0 : 1;
		uint8_t CLK_SPI1 : 1;
		uint8_t CLK_PWMX : 1;
		uint8_t CLK_I2C : 1;
		uint8_t CLK_USB : 1;
		uint8_t CLK_USB2 : 1;
		uint8_t : 1;
		uint8_t CLK_BLE : 1;
	};
	uint8_t byte[2];
}SLP_CLK_OFF_t;


typedef union SLP_WAKE_CTRL {
	struct {
		uint8_t USB_WAKE : 1;
		uint8_t USB2_WAKE : 1;
		uint8_t : 1;
		uint8_t RTC_WAKE : 1;
		uint8_t GPIO_WAKE : 1;
		uint8_t BAT_WAKE : 1;
		uint8_t EV_MODE : 1;
		uint8_t : 1;
	};
	uint8_t byte;
}SLP_WAKE_CTRL_t;

typedef union SLP_POEWR_CTRL {
	struct {
		uint8_t WAKE_DLY_MOD : 2;
		uint8_t : 2;
		uint8_t CLK_RAMX : 1;
		uint8_t CLK_RAM2K : 1;
		uint8_t RAM_RET_LV : 1;
		uint8_t : 1;
	};
	uint8_t byte;
}SLP_POEWR_CTRL_t;

typedef union POWER_PLAN {
	struct {
		uint16_t XROM : 1;
		uint16_t RAM2K : 1;
		uint16_t CORE : 1;
		uint16_t EXTEND : 1;
		uint16_t RAM30K : 1;
		uint16_t : 1;
		uint16_t MUST_0 : 1;
		uint16_t SYS_EN : 1;

		uint16_t LDO_EN : 1;
		uint16_t DCDC_EN : 1;
		uint16_t DCDC_PRE : 1;
		uint16_t MUST_0010 : 4;
		uint16_t PLAN_EN : 1;
	};
	uint16_t Group;
}POWER_PLAN_t;

typedef union AUX_POWER_ADJ {
	struct {
		uint16_t ULPLDO_ADJ : 3;
		uint16_t MUST_ORIGIN : 3;
		uint16_t : 1;
		uint16_t DCDC_CHARGE : 1;
		
		uint16_t : 8;
	};
	uint16_t Group;
}AUX_POWER_ADJ_t;

typedef union BAT_DET_CTRL {
	struct {
		uint8_t BAT_DET_EN : 1;
		uint8_t BAT_MON_IE : 1;
		uint8_t BAT_LOWER_IE : 1;
		uint8_t BAT_LOW_IE : 1;
		uint8_t : 4;
	};
	struct {
		uint8_t BAT_LOW_VTHX : 1;
		uint8_t : 7;
	};
	uint8_t byte;
}BAT_DET_CTRL_t;

typedef union BAT_DET_CFG {
	struct {
		uint8_t BAT_LOW_VTH : 2;
		uint8_t : 6;
	};
	uint8_t byte;
}BAT_DET_CFG_t;

typedef union BAT_STATUS {
	struct {
		uint8_t BAT_STAT_LOWER : 1;
		uint8_t BAT_STAT_LOW : 1;
		uint8_t : 6;
	};
	uint8_t byte;
}BAT_STATUS_t;

#define x16_SLP_CLK_OFF			(*(volatile SLP_CLK_OFF_t*)		0x4000100C)
#define x8_SLP_CLK_OFF0				x16_SLP_CLK_OFF.byte[0]
#define x8_SLP_CLK_OFF1				x16_SLP_CLK_OFF.byte[1]
#define x8_SLP_WAKE_CTRL			(*(volatile SLP_WAKE_CTRL_t*)		0x4000100E)
#define x8_SLP_POWER_CTRL			(*(volatile SLP_POEWR_CTRL_t*)		0x4000100F)
#define x16_POWER_PLAN				(*(volatile POWER_PLAN_t*)			0x40001020)
#define x16_AUX_POWER_ADJ			(*(volatile AUX_POWER_ADJ_t*)		0x40001022)
#define x8_BAT_DET_CTRL				(*(volatile BAT_DET_CTRL_t*)		0x40001024)
#define x8_BAT_DET_CFG				(*(volatile BAT_DET_CFG_t*)			0x40001025)
#define x8_BAT_STATUS				(*(volatile BAT_STATUS_t*)			0x40001026)

#endif // !__CH58xPMUSFR__
