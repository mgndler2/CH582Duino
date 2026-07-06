#include "CH58x_I2C.h"

uint8_t I2C_ModuleX::Module = 0;

I2C_ModuleX& Base_I2C(uint8_t id) {
	static I2C_ModuleX obj[I2C_Circuits];
	return obj[id];
}

void I2C_EventHandler(uint8_t event);
void I2C_ErrorHandler(uint8_t error);

uint8_t I2C_Base::PourSlave(uint8_t* data) {
	uint8_t len = Base_I2C(Circuit_Id).SlaveRx.Index;
	Base_I2C(Circuit_Id).SlaveRx.Index = 0;
	for (int i = 0;i < len;i++) {
		data[i] = Base_I2C(Circuit_Id).SlaveRxBuf[i];
	}
	return len;
}

uint8_t I2C_Base::FillSlave(uint8_t* data, uint8_t length) {
	int i = 0;
	while ((Base_I2C(Circuit_Id).SlaveTx.Length + length) <= 32) {
		Base_I2C(Circuit_Id).SlaveTxBuf[Base_I2C(Circuit_Id).SlaveTx.Length++] = data[i];
		i++;
	}
	Base_I2C(Circuit_Id).SlaveTx.Index = 0;
	return i;
}

uint8_t I2C_Base::FillSlave(uint8_t data) {
	if (Base_I2C(Circuit_Id).SlaveTx.Length >= 32) return 0;
	Base_I2C(Circuit_Id).SlaveTxBuf[Base_I2C(Circuit_Id).SlaveTx.Length++] = data;
	Base_I2C(Circuit_Id).SlaveTx.Index = 0;
	return 1;
}

I2C_Base::I2C_Base() :
	_I2C(I2C),Circuit_Id(0) {
}

I2C_Base::I2C_Base(volatile I2C_Module_t& mod, uint8_t Circuit) :
	_I2C(mod), Circuit_Id(Circuit) {
}

void I2C_Base::I2C_init(uint32_t clock) {
	SoftWareReset();
	MasterClock = clock;
	setClock(clock);
	
	_I2C.CTRL2.ITEVTEN = 1;
	_I2C.CTRL2.ITBUFEN = 1;
	_I2C.CTRL2.ITERREN = 1;
	PFIC_SetPriority(I2C_IRQn, 0x00);
	PFIC_EnableIRQ(I2C_IRQn);
	Base_I2C(Circuit_Id).state = 0;
	
	_I2C.CTRL1.PE = 1;
	setACK();
}

void I2C_Base::setAddr1(int addr, uint8_t is10bitAddr) {
	I2C_OADDR1_t tmp = { .raw = 0, };
	tmp.MUST1 = 1;
	if (is10bitAddr) {
		tmp.ADDMODE = 1;
		tmp.ADD9_8 = (addr & 0x300) >> 8;
		tmp.ADD7_0 = (uint8_t)(addr & 0xFF);
	}
	else {
		tmp.ADDMODE = 0;
		tmp.ADD7_1 = (uint8_t)(addr & 0x7F);
	}
	_I2C.OADDR1.raw = tmp.raw;
}

void I2C_Base::setAddr2(uint8_t addr, uint8_t Enable) {
	_I2C.OADDR2.ADD2 = addr & 0x7F;
	_I2C.OADDR2.ENDUAL = Enable;
}

void I2C_Base::setClock(uint32_t Clock) {
	uint32_t SysClock = CPU_CLK.GetSysClock();
	uint32_t CLK_MHz = SysClock / 1000000;
	uint8_t _PE = _I2C.CTRL1.PE;
	_I2C.CTRL1.PE = 0;
	
	_I2C.CTRL2.FREQ = CLK_MHz;
	//Trise should be more than 0.2us. And TRISE = Trise(us) * Fhclk(MHz) + 1;
	_I2C.RTR.TRISE = CLK_MHz * 300 / 1000 + 1;

	if (Clock <= 140000) {
		_I2C.CKCFGR.F_S = 0;
	}
	else if (Clock <= 380000) {
		_I2C.CKCFGR.F_S = 1;
		_I2C.CKCFGR.DUTY = 0;
		_I2C.CKCFGR.CCR = SysClock / 3 / Clock;
	}
	else {
		_I2C.CKCFGR.F_S = 1;
		_I2C.CKCFGR.DUTY = 1;
		_I2C.CKCFGR.CCR = SysClock / 25 / Clock;
	}
	_I2C.CTRL1.PE = _PE;
}

/*
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 *          sendStop: boolean indicating whether or not to send a stop at the end
 * Output   0 .. success
 *          1 .. length too long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout
 */
uint8_t I2C_Base::writeTo(uint16_t address, uint8_t* data, int8_t length, uint8_t wait, uint8_t sendStop, uint8_t is10BitAddr) {
	using namespace I2C_Status;
	if (32 < length) {
		return 1;
	}
	
	uint32_t EnterTime = micros();
	while (Base_I2C(Circuit_Id).state != TWI_READY) {
		if (timeout_us > 0 && (micros() - EnterTime) > timeout_us) {
			handleTimeout(flag_resetOntimeout);
			return 5;
		}
	}
	Base_I2C(Circuit_Id).state = TWI_MTX;
	Base_I2C(Circuit_Id).appendStop = sendStop;
	for (int i = 0; i < length; i++)
	{
		Base_I2C(Circuit_Id).MasterBuf[i] = data[i];
	}
	Base_I2C(Circuit_Id).setMasterTable(length, 0);

	if (is10BitAddr) {
		Base_I2C(Circuit_Id).F8bit = 0xF0 + (address + 0x300) >> 7 | 0;
		Base_I2C(Circuit_Id).L8bit = address & 0xFF;
	}
	else {
		Base_I2C(Circuit_Id).F8bit = (address & 0x7F) << 1 | 0;
	}
	
	_I2C.CTRL1.START = 1;
	_I2C.CTRL2.ITBUFEN = 1;
	_I2C.CTRL2.ITEVTEN = 1;

	EnterTime = micros();
	while (wait && (Base_I2C(Circuit_Id).state == TWI_MTX)) {
		if (timeout_us > 0 && (micros() - EnterTime) > timeout_us) {
			handleTimeout(flag_resetOntimeout);
			return 5;
		}
	}

	if (Base_I2C(Circuit_Id).ErrCode.Error == 0)
		return 0;
	else if (Base_I2C(Circuit_Id).ErrCode.Raw == MTX_ADDR_NAK)
		return 2;
	else if (Base_I2C(Circuit_Id).ErrCode.Raw == MTX_DATA_NAK)
		return 3;
	else
		return 4;
}

uint8_t I2C_Base::readFrom(uint16_t address, uint8_t* data, uint8_t length, uint8_t sendStop, uint8_t is10BitAddr) {
	if (32 < length) {
		return 0;
	}
	
	uint32_t EnterTime = micros();
	while (Base_I2C(Circuit_Id).state != TWI_READY) {
		if (timeout_us > 0 && (micros() - EnterTime) > 100 * timeout_us) {
			handleTimeout(flag_resetOntimeout);
			return 0;
		}
	}

	Base_I2C(Circuit_Id).appendStop = sendStop;
	Base_I2C(Circuit_Id).setMasterTable(length, 0);

	if (is10BitAddr) {
		Base_I2C(Circuit_Id).F8bit = 0xF0 + (address + 0x300) >> 7 | 1;
		Base_I2C(Circuit_Id).L8bit = address & 0xFF;
	}
	else {
		Base_I2C(Circuit_Id).F8bit = (address & 0x7F) << 1 | 1;
	}
	setACK();
	Base_I2C(Circuit_Id).state = TWI_MRX;

	_I2C.CTRL1.START = 1;
	while (_I2C.STAR1.SB == 0);
	_I2C.CTRL2.ITBUFEN = 1;
	_I2C.CTRL2.ITEVTEN = 1;

	EnterTime = micros();
	while (Base_I2C(Circuit_Id).state == TWI_MRX) {
		if (timeout_us > 0 && (micros() - EnterTime) > timeout_us) {
			handleTimeout(flag_resetOntimeout);
			return 0;
		}
	}

	if (Base_I2C(Circuit_Id).MasterStep() < length) {
		length = Base_I2C(Circuit_Id).Master.Index;
	}	
	for (int i = 0; i < length; i++) {
		data[i] = Base_I2C(Circuit_Id).MasterBuf[i];
	}
	Base_I2C(Circuit_Id).setMasterTable(0, 0);

	return length;
}

void I2C_Base::handleTimeout(uint8_t doReset) {
	flag_timeout = 1;
	if (doReset) {
		uint16_t ADDR1 = _I2C.OADDR1.raw;
		uint16_t ADDR2 = _I2C.OADDR2.raw;
		I2C_init(MasterClock);
		_I2C.OADDR1.raw = ADDR1;
		_I2C.OADDR2.raw = ADDR2;
	}
}

uint8_t I2C_Base::manageTimeOutFlag(uint8_t ClearFlag) {
	uint8_t tmp = flag_timeout;
	if (ClearFlag) flag_timeout = 0;
	return tmp;
}


extern "C"
__INTERRUPT
void I2C_IRQHandler(void) {	
	//Serial1.println("");
	if (x16_I2C_CTRL2.ITEVTEN && x16_I2C_STAR1.EVENT) {
		I2C_EventHandler(x16_I2C_STAR1.EVENT);
	}
	if (x16_I2C_CTRL2.ITERREN && x16_I2C_STAR1.ERROR) {
		I2C_ErrorHandler(x16_I2C_STAR1.ERROR);
	}
}

void I2C_EventHandler(uint8_t event) {
	using namespace I2C_Status;
	const uint8_t CircuitId = 0;
	uint8_t is10bitAddr = 0;
	if (event & SB) {
		x16_I2C_DATAR.DATAR = Base_I2C(CircuitId).F8bit;
		//Serial1.println("SB");
	}
	else if (event & STOPF) {
		x16_I2C_CTRL1.PE = 1;
		if (Base_I2C(CircuitId).onSlaveReceived != nullptr) {
			Base_I2C(CircuitId).onSlaveReceived(Base_I2C(CircuitId).ctx);
		}
	}
	else if (event & ADDR) {
		if (x16_I2C_STAR2.MSL == 0)
		{
			if (x16_I2C_STAR2.TRA) {
				Base_I2C(CircuitId).state = TWI_STX;
				if (Base_I2C(CircuitId).onSlaveTransmit != nullptr) {
					Base_I2C(CircuitId).onSlaveTransmit(Base_I2C(CircuitId).ctx);
				}
			}
			else {
				Base_I2C(CircuitId).state = TWI_SRX;
			}
		}
		else if (Base_I2C(CircuitId).state == TWI_MRX) {
			if (is10bitAddr == 1) {
				x16_I2C_DATAR.DATAR = Base_I2C(CircuitId).F8bit;
				x16_I2C_CTRL1.START = 1;
			}
			else if (Base_I2C(CircuitId).Master_remain() == 1 && Base_I2C(CircuitId).appendStop == 1) {
				Base_I2C(CircuitId).setNAK();
			}
		}
		is10bitAddr = 0;
		//Serial1.println("ADD");
	}
	else if (event & BTF) {
		if (Base_I2C(CircuitId).appendStop==0) {
			x16_I2C_CTRL2.ITBUFEN = 0;
			x16_I2C_CTRL2.ITEVTEN = 0;
		}
		else {

		}
		Base_I2C(CircuitId).state = TWI_READY;
		Serial1.println("BTF!");
	}
	else if (event & ADD10) {
		x16_I2C_CTRL1.PE = 1;
		x16_I2C_DATAR.DATAR = Base_I2C(CircuitId).L8bit;
		is10bitAddr = 1;
		//Serial1.println("ADD10");
	}
	uint8_t data = 0;
	if (x16_I2C_STAR1.RxNE) {
		if (Base_I2C(CircuitId).state == TWI_MRX) {
			if (Base_I2C(CircuitId).Master_remain()) {
				data = x16_I2C_DATAR.DATAR;
				uint8_t remain = Base_I2C(CircuitId).MasterShiftIn(data);
				if (remain == 0 && Base_I2C(CircuitId).appendStop == 1) {
					Base_I2C(CircuitId).setNAK(); Base_I2C(CircuitId).state = TWI_READY;
				}
			}
		}
		if (Base_I2C(CircuitId).state == TWI_SRX) {
			if (Base_I2C(CircuitId).Slave_remain()) {
				data = x16_I2C_DATAR.DATAR;
				Base_I2C(CircuitId).SlaveShiftIn(data);

				if (Base_I2C(CircuitId).Slave_remain() == 0) Base_I2C(CircuitId).setNAK();
			}
		}
		//Serial1.println("RxNE");
	}
	else if (x16_I2C_STAR1.TxE) {
		if (Base_I2C(CircuitId).state == TWI_MTX) {
			if (Base_I2C(CircuitId).Master_remain()) {
				data = Base_I2C(CircuitId).MasterShiftOut();
				x16_I2C_DATAR.DATAR = data;

				if (Base_I2C(CircuitId).appendStop == 1 && Base_I2C(CircuitId).Master_remain() == 0) x16_I2C_CTRL1.STOP;
			}
		}
		else if (Base_I2C(CircuitId).state == TWI_STX) {
			if (Base_I2C(CircuitId).Slave_remain()) {
				data = Base_I2C(CircuitId).SlaveShiftOut();
				x16_I2C_DATAR.DATAR = data;
			}
			else {
				x16_I2C_DATAR.DATAR = 0xFF;
			}
		}
		//Serial1.println("TxE");
	}
	//Serial1.println("EVENT");
}

void I2C_ErrorHandler(uint8_t error) {
	using namespace I2C_Status;
	const uint8_t CircuitId = 0;
	static uint8_t is10bitAddr = 0;
	if (error & BERR) {
		Base_I2C(CircuitId)._I2C.STAR1.BERR = 0;
	}
	else if (error & ARLO) {
		Base_I2C(CircuitId)._I2C.STAR1.ARLO = 0;
	}
	else if (error & AF) {
		Base_I2C(CircuitId)._I2C.STAR1.AF = 0;
		Base_I2C(CircuitId).ErrCode.Error = 1;
		if (Base_I2C(CircuitId).state == TWI_MTX) {
			if (Base_I2C(CircuitId).MasterStep() == 0) {		//Addr NACK
				Base_I2C(CircuitId).ErrCode.State = 0b11;
				Base_I2C(CircuitId).ErrCode.Stage = 0;
				Base_I2C(CircuitId).ErrCode.Type = 3;
			}
			else {												//Data NACK
				Base_I2C(CircuitId).ErrCode.State = 0b11;
				Base_I2C(CircuitId).ErrCode.Stage = 1;
				Base_I2C(CircuitId).ErrCode.Type = 3;
			}
		}
		if (Base_I2C(CircuitId).state == TWI_MRX) {
			Base_I2C(CircuitId).ErrCode.State = 0b10;
			Base_I2C(CircuitId).ErrCode.Stage = 0;
			Base_I2C(CircuitId).ErrCode.Type = 3;
		}
	}
	else if (error & OVR) {
		Base_I2C(CircuitId)._I2C.STAR1.OVR = 0;
	}
	else if (error & PECERR) {
		Base_I2C(CircuitId)._I2C.STAR1.PECERR = 0;
	}
	else if (error & TIMEOUT) {
		Base_I2C(CircuitId)._I2C.STAR1.TIMEOUT = 0;
	}
	else if (error & SMBALERT) {
		Base_I2C(CircuitId)._I2C.STAR1.SMBALERT = 0;
	}
	//Serial1.println("ERROR");
}