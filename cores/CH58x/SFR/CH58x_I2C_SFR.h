#pragma once

#ifndef __CH58xI2CSFR__
#define __CH58xI2CSFR__

#include "CH58x_SFR_TOP_LVL.h"
//0x00
typedef union I2C_CTRL1 {
	struct {
		uint16_t PE : 1;
		uint16_t SMBUS : 1;
		uint16_t : 1;
		uint16_t SMBTYPE : 1;
		uint16_t ENARP : 1;
		uint16_t ENPEC : 1;
		uint16_t ENGC : 1;
		uint16_t NOSTRETCH : 1;

		uint16_t START : 1;
		uint16_t STOP : 1;
		uint16_t ACK : 1;
		uint16_t POS : 1;
		uint16_t PEC : 1;
		uint16_t ALERT : 1;
		uint16_t : 1;
		uint16_t SWRST : 1;
	};
	uint16_t raw;
}I2C_CTRL1_t;
//0x04
typedef union I2C_CTRL2 {
	struct {
		uint16_t FREQ : 6;
		uint16_t : 2;

		uint16_t ITERREN : 1;
		uint16_t ITEVTEN : 1;
		uint16_t ITBUFEN : 1;
		uint16_t : 5;
	};
	uint16_t raw;
}I2C_CTRL2_t;
//0x08
typedef union I2C_OADDR1 {
	struct {
		uint16_t ADD0 : 1;
		uint16_t ADD7_1 : 7;

		uint16_t ADD9_8: 2;
		uint16_t : 4;
		uint16_t MUST1 : 1;
		uint16_t ADDMODE : 1;
	};
	struct {
		uint16_t ADD7_0 : 8;

		uint16_t : 8;
	};
	uint16_t raw;
}I2C_OADDR1_t;
//0x0C
typedef union I2C_OADDR2 {
	struct {
		uint16_t ENDUAL : 1;
		uint16_t ADD2 : 7;

		uint16_t : 8;
	};
	uint16_t raw;
}I2C_OADDR2_t;
//0x10
typedef union I2C_DATAR {
	struct {
		uint16_t DATAR : 8;
		uint16_t : 8;
	};
	uint16_t raw;
}I2C_DATAR_t;
//0x14
typedef union I2C_STAR1 {
	struct {
		uint16_t SB : 1;
		uint16_t ADDR : 1;
		uint16_t BTF : 1;
		uint16_t ADD10 : 1;
		uint16_t STOPF : 1;
		uint16_t : 1;
		uint16_t RxNE : 1;
		uint16_t TxE : 1;

		uint16_t BERR : 1;
		uint16_t ARLO : 1;
		uint16_t AF : 1;
		uint16_t OVR : 1;
		uint16_t PECERR : 1;
		uint16_t : 1;
		uint16_t TIMEOUT: 1;
		uint16_t SMBALERT : 1;
	};
	struct {
		uint16_t EVENT : 8;
		uint16_t ERROR : 8;
	};
	uint16_t raw;
}I2C_STAR1_t;
//0x18
typedef union I2C_STAR2 {
	struct {
		uint16_t MSL : 1;
		uint16_t BUSY : 1;
		uint16_t TRA : 1;
		uint16_t : 1;
		uint16_t GENCALL : 1;
		uint16_t SMBFAULT : 1;
		uint16_t SMBHOST : 1;
		uint16_t DUALF : 1;

		uint16_t : 8;
	};
	uint16_t raw;
}I2C_STAR2_t;
//0x1C
typedef union I2C_CKCFGR {
	struct {
		uint16_t CCR : 12;
		uint16_t : 2;
		uint16_t DUTY : 1;
		uint16_t F_S: 1;
	};
	uint16_t raw;
}I2C_CKCFGR_t;
//0x20
typedef union I2C_RTR {
	struct {
		uint16_t TRISE : 6;
		uint16_t : 10;
	};
	uint16_t raw;
}I2C_RTR_t;

typedef union I2C_Module {
	struct {
		I2C_CTRL1_t		CTRL1;		//0x00-01
		uint16_t : 16;				//0x02-03//
		I2C_CTRL2_t		CTRL2;		//0x04-05
		uint16_t : 16;				//0x06-07//
		I2C_OADDR1_t	OADDR1;		//0x08-09
		uint16_t : 16;				//0x0A-0B//
		I2C_OADDR2_t	OADDR2;		//0x0C-0D
		uint16_t : 16;				//0x0E-0F//
		I2C_DATAR_t		DATAR;		//0x10-11
		uint16_t : 16;				//0x12-13//
		I2C_STAR1_t		STAR1;		//0x14-15
		uint16_t : 16;				//0x16-17//
		I2C_STAR2_t		STAR2;		//0x18-19
		uint16_t : 16;				//0x1A-1B//
		I2C_CKCFGR_t	CKCFGR;		//0x1C-1D
		uint16_t : 16;				//0x1E-1F//
		I2C_RTR_t		RTR;		//0x20-21
		uint16_t : 16;				//0x22-23//
	};
}I2C_Module_t;
//
// I2C
//
#define I2C			(*(volatile I2C_Module_t*)		0x40004800)
#define x16_I2C_CTRL1	(*(volatile I2C_CTRL1_t*)		0x40004800)
/*-----0x40004802-03 is RSVD-----*/
#define x16_I2C_CTRL2	(*(volatile I2C_CTRL2_t*)		0x40004804)
/*-----0x40004806-07 is RSVD-----*/
#define x16_I2C_OADDR1	(*(volatile I2C_OADDR1_t*)		0x40004808)
/*-----0x4000480A-B3 is RSVD-----*/
#define x16_I2C_OADDR2	(*(volatile I2C_OADDR2_t*)		0x4000480C)
/*-----0x4000480E-0F is RSVD-----*/
#define x16_I2C_DATAR	(*(volatile I2C_DATAR_t*)		0x40004810)
/*-----0x40004812-13 is RSVD-----*/
#define x16_I2C_STAR1	(*(volatile I2C_STAR1_t*)		0x40004814)
/*-----0x40004802-03 is RSVD-----*/
#define x16_I2C_STAR2	(*(volatile I2C_STAR2_t*)		0x40004818)
/*-----0x4000481A-1B is RSVD-----*/
#define x16_I2C_CKCFGR	(*(volatile I2C_CKCFGR_t*)		0x4000481C)
/*-----0x4000481E-1F is RSVD-----*/
#define x16_I2C_RTR		(*(volatile I2C_RTR_t*)			0x40004820)
/*-----0x40004822-23 is RSVD-----*/

#endif // !__CH58xI2CSFR__
