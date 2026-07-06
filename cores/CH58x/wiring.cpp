/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "./SFR/CH58x_SYSTICK_SFR.h"
#include "./SFR/CH58x_PMU_SFR.h"
#include "./HAL/CH58x_SYS.h"
#include "./HAL/HAL.h"
//#include "./PERIPH/CH58x_sui.h"
#include "CDC.h"

uint32_t Ftt = 0;

#ifdef __cplusplus
extern "C" {
#endif
	volatile unsigned long t_millis = 0;
	unsigned long millis()
	{
		volatile unsigned long t;
		t = t_millis;		
		return t;
	}

	unsigned long micros() {
		unsigned long t;
		t = t_millis * 1000 + SySTick.STK_CNTL.u16L / 6;
		return t ;
	}

	void delay(unsigned long ms)
	{
		uint32_t inTime = millis();
		while ((millis() - inTime) < ms) {
			if (need_serial_state_notify) {
				need_serial_state_notify = 0;
				uint8_t* EP1_Buffer = USBDevice.getEpBuffer(1);
				memcpy(EP1_Buffer, serial_state_notify, 10);
				x8_UEP1_T_LEN.length = 10;
				PFIC_DisableIRQ(USB_IRQn);
				R8_UEP1_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
				PFIC_EnableIRQ(USB_IRQn);
				
			}
			TMOS_SystemProcess();
		};
	}

	void delayMicroseconds(unsigned int us)
	{
		uint32_t inTime = micros();
		while ((micros() - inTime) < us);
	}

#if(F_CPU == 60000000UL)
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_PLL_60MHz;
#elif(F_CPU == 48000000UL)
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_PLL_48MHz;
#elif(F_CPU == 32000000UL)
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_PLL_32MHz;
#elif(F_CPU == 24000000UL)
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_PLL_24MHz;
#elif(F_CPU == 16000000UL)
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_HSE_16MHz;
#else
	ns_SYS::HCLK_CLKSOURCE HCLK = ns_SYS::CLK_SOURCE_PLL_48MHz;
#endif


	void init(void)
	{
		CPU_CLK.SetSysClock(HCLK);
		SySTick.STK_CMPLR.pad = CPU_CLK.GetSysClock() / 8000;  //F_cpu / 8 / 1kHz = 6000;
		SySTick.STK_CMPHR.pad = 0x00000000;
		
#if(DCDC == 1)
		sys_safe_access_enable();
		x16_POWER_PLAN.DCDC_PRE = 1;
		x16_POWER_PLAN.DCDC_EN = 1;
		sys_safe_access_disable();
#elif(DCDC == 0)
		sys_safe_access_enable();
		x16_POWER_PLAN.DCDC_PRE = 0;
		x16_POWER_PLAN.DCDC_EN = 0;
		sys_safe_access_disable();
#endif

		for (int i = 0; i < 1200; i++)
		{
			_nop();_nop();
		}
		//PFIC_SetPriority(SysTick_IRQn, 10);
		PFIC_EnableIRQ(SysTick_IRQn);
		SySTick.STK_CTRL.pad = 0x002B;
	}

	__INTERRUPT
		void SysTick_Handler(void) {
		t_millis++;
		SysTick->SR = 0;
	}
#ifdef __cplusplus
}
#endif