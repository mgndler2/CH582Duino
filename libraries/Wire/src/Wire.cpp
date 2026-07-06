#include "Wire.h"

TwoWire::TwoWire() :I2C_Base(I2C) {}
TwoWire::TwoWire(I2C_Module_t& mod, uint8_t pinRemap) :
	I2C_Base(mod) { 
	pinMap(pinRemap);
}

void TwoWire::pinMap(uint8_t pinRemap) {
	x16_PIN_ALTERNATE.PIN_I2C = pinRemap;
	if (pinRemap) {
		pinMode(PB21, INPUT_PULLUP);	//SCL_
		pinMode(PB20, INPUT_PULLUP);	//SDA_
	}
	else {
		pinMode(PB13, INPUT_PULLUP);	//SCL
		pinMode(PB12, INPUT_PULLUP);	//SDA
	}
}

void TwoWire::onDataArrived() {
	if (user_onReceive == nullptr) return;
	RX.Length = PourSlave(RxBuf);
	RX.Index = 0;

	user_onReceive(RX.Length);
}

void TwoWire::onDataRequest() {
	if (user_onRequest == nullptr) return;

	Base_I2C(Circuit_Id).setSlaveTable(0, 0, 0); //Clear Slave buffer index

	user_onRequest();
}

void TwoWire::begin() {
	RX.Index = 0;
	RX.Length = 0;

	TX.Index = 0;
	TX.Length = 0;

	Base_I2C(Circuit_Id).onSlaveTransmit = txThunk;
	Base_I2C(Circuit_Id).onSlaveReceived = rxThunk;
	Base_I2C(Circuit_Id).ctx = this;

	I2C_init((uint16_t)100000);
}

void TwoWire::begin(int address, uint8_t is10BitAddr) {
	begin();
	setAddr1(address, is10BitAddr);
}

void TwoWire::beginTransmission(int address, uint8_t is10BitAddr) {
	transmitting = 1;

	targetAddr = address;
	targetIs10bit = is10BitAddr;
	
	TX.Index = 0;
	TX.Length = 0;
}

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
	uint8_t ret = writeTo(targetAddr, TxBuf, TX.Length, 1, sendStop, targetIs10bit);
	TX.Index = 0;
	TX.Length = 0;
	transmitting = 0;
	return ret;
}

uint8_t TwoWire::endTransmission(void)
{
	return endTransmission(1);
}

uint8_t TwoWire::requestFrom(int address, uint8_t dataLen, uint32_t command, uint8_t CMDLen, uint8_t sendStop, uint8_t is10BitAddr) {
	
	beginTransmission(address, is10BitAddr);
	while (CMDLen!=0) {
		CMDLen--;
		write(command >> (8 * CMDLen));
	}
	endTransmission(0);

	uint8_t read = readFrom(address, RxBuf, dataLen, sendStop, is10BitAddr);
	RX.Index = 0;
	RX.Length = read;
	return read;
}

uint8_t TwoWire::write(uint8_t data) {
	if (transmitting)
	{
		if (TX.Length >= 32) return 0;

		TxBuf[TX.Length] = data;
		TX.Length++;

		TX.Index = TX.Length;
	}
	else {
		return FillSlave(data);
	}
	return 1;
}

uint8_t TwoWire::write(uint8_t* data, int length) {
	int i = 0;
	if (transmitting)
	{
		if (TX.Length >= 32) return 0;
		
		while ((TX.Length + length) <= 32) {
			TxBuf[TX.Length++] = data[i];
			i++;
		}
	}
	else {
		i = FillSlave(data, length);
	}
	return i;
}

uint8_t TwoWire::pullFromSlaveRx() {
	RX.Index = 0;
	return RX.Length = PourSlave(RxBuf);
}

uint8_t TwoWire::read() {
	if (RX.Index >= RX.Length) return 0xFF;

	uint8_t data = RxBuf[RX.Index];
	RX.Index++;
	return data;
}

uint8_t TwoWire::peek() {
	if (RX.Index >= RX.Length) return 0xFF;

	uint8_t data = RxBuf[RX.Index];
	return data;
}

uint8_t TwoWire::available() {
	return RX.Length - RX.Index;
}