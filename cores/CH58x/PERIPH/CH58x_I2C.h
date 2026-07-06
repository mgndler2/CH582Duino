#ifndef __CH58x_I2C_H__
#define __CH58x_I2C_H__
#include "Arduino.h"
#include "CH58x_I2C_SFR.h"
#include "CH58x_GPIO_SFR.h"
#include "CH58x_SYS.h"
#include "USBAPI.h"

#define TWI_READY 0
#define TWI_MRX   1
#define TWI_MTX   2
#define TWI_SRX   3
#define TWI_STX   4


#if(dual_I2C)
#define I2C_Circuits 2
#else
#define I2C_Circuits 1
#endif

namespace I2C_Status {
	enum EventFlag {
		NONE = 0x0000,
		SB = 0x0001,
		ADDR = 0x0002,
		BTF = 0x0004,
		ADD10 = 0x0008,
		STOPF = 0x0010
	};
	enum ErrorFlag {
		BERR = 0x01,
		ARLO = 0x02,
		AF = 0x04,
		OVR = 0x08,
		PECERR = 0x10,
		TIMEOUT = 0x40,
		SMBALERT = 0x80
	};
	enum ErrorType {
		MTX_ADDR_NAK = 0b11100011,
		MTX_DATA_NAK = 0b11110011,
		MRX_ADDR_NAK = 0b11000011
	};
}

class I2C_ModuleX {
private:
	struct BufTable {
		int8_t Length;
		int8_t Index;
	};
	typedef union I2C_ERROR {
		struct
		{
			uint8_t Type : 4;
			uint8_t Stage : 1;
			uint8_t Transmit : 1;
			uint8_t Master : 1;
			uint8_t Error : 1;
		};
		struct
		{
			uint8_t : 5;
			uint8_t State : 2;
			uint8_t : 1;
		};
		uint8_t Raw;
	}I2C_ERROR_t;

	__aligned_four__ volatile uint8_t SlaveTxBuf[32];
	__aligned_four__ volatile uint8_t SlaveRxBuf[32];
	__aligned_four__ volatile uint8_t MasterBuf[32];
	BufTable volatile Master;
	BufTable volatile SlaveTx;
	BufTable volatile SlaveRx;

	friend class I2C_Base;
public:
	I2C_ModuleX() :
		_I2C(getModule(Module))
	{
		Module++;
	}

	inline void setACK() {
		_I2C.CTRL1.ACK = 1;
	}

	inline void setNAK() {
		_I2C.CTRL1.ACK = 0;
	}

	uint8_t MasterShiftOut() {
		uint8_t data = MasterBuf[Master.Index];
		Master.Index++;
		return data;
	}

	uint8_t MasterShiftIn(uint8_t data) {
		MasterBuf[Master.Index] = data;
		Master.Index++;
		return Master.Length - Master.Index;
	}

	uint8_t MasterStep() {
		return Master.Index;
	}

	inline uint8_t Master_remain() {
		return Master.Length - Master.Index;
	}

	uint8_t SlaveShiftOut() {
		uint8_t data = SlaveTxBuf[SlaveTx.Index];
		SlaveTx.Index++;
		return data;
	}

	uint8_t SlaveShiftIn(uint8_t data) {
		SlaveRxBuf[SlaveRx.Index] = data;
		SlaveRx.Index++;
		return 32 - SlaveRx.Index;
	}

	uint8_t Slave_remain() {
		if (state == TWI_SRX) {
			return 32 - SlaveRx.Index;
		}
		else if (state == TWI_STX) {
			return SlaveTx.Length - SlaveTx.Index;
		}
		return 0;
	}

	void setMasterTable(int8_t Length, int8_t Index) {
		if (Length != -1) Master.Length = Length;
		if (Index != -1) Master.Index = Index;
	}

	void setSlaveTable(int8_t Length, int8_t Index, uint8_t tx_rx) {
		if (Length != -1) {
			if (tx_rx) {
				SlaveRx.Length = Length;
			}
			else {
				SlaveTx.Length = Length;
			}
		}
		if (Index != -1) {
			if (tx_rx) {
				SlaveRx.Index = Index;
			}
			else {
				SlaveTx.Index = Index;
			}
		}
	}

	Callback onSlaveTransmit = nullptr;
	Callback onSlaveReceived = nullptr;
	void* ctx;

	volatile uint8_t appendStop;
	volatile uint8_t F8bit;
	volatile uint8_t L8bit;
	volatile uint8_t state;

	volatile I2C_ERROR_t ErrCode;


	static uint8_t Module;
	static volatile I2C_Module_t& getModule(uint8_t id) {
		if (id == 0)
			return I2C;
#if(I2C_Circuits == 2)
		else
			return I2C1;
#endif
		return I2C;
	}
	volatile I2C_Module_t& _I2C;
};

class I2C_Base {
public:
	I2C_Base();
	I2C_Base(volatile I2C_Module_t& mod, uint8_t Circuit = 0);
	void setClock(uint32_t Clock);
	void setAddr1(int addr, uint8_t is10bitAddr = 0);
	void setAddr2(uint8_t addr, uint8_t Enable = 0);
	void I2C_init(uint32_t clock);

	uint8_t writeTo(uint16_t address, uint8_t* data, int8_t length, uint8_t wait, uint8_t sendStop, uint8_t is10BitAddr = 0);
	uint8_t readFrom(uint16_t address, uint8_t* data, uint8_t length, uint8_t sendStop, uint8_t is10BitAddr = 0);

	inline void setACK() {
		_I2C.CTRL1.ACK = 1;
	}

	inline void setNAK() {
		_I2C.CTRL1.ACK = 0;
	}
	

protected:
	volatile I2C_Module_t& _I2C;
	uint8_t Circuit_Id;

	inline void SoftWareReset() {
		_I2C.CTRL1.SWRST = 1;
		_nop();
		_I2C.CTRL1.SWRST = 0;
	}

	uint8_t PourSlave(uint8_t* data);
	uint8_t FillSlave(uint8_t* data, uint8_t length);
	uint8_t FillSlave(uint8_t data);
private:
	void handleTimeout(uint8_t doReset);
	uint8_t manageTimeOutFlag(uint8_t ClearFlag);
	uint32_t MasterClock = 100000;
	uint32_t timeout_us = 250000ul;
	uint8_t flag_timeout = 0;
	uint8_t flag_resetOntimeout = 0;
};
I2C_ModuleX& Base_I2C(uint8_t id);

#endif // !__CH58x_I2C_H__
