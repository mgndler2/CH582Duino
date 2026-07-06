#ifndef TwoWire_h
#define TwoWire_h

#include "CH58x_I2C.h"

class TwoWire : public I2C_Base {
public:
	TwoWire();
	TwoWire(I2C_Module_t& mod, uint8_t pinRemap = 0);
	void pinMap(uint8_t pinRemap);
	void begin();
	void begin(int address, uint8_t is10BitAddr = 0);
	void beginTransmission(int address, uint8_t is10BitAddr = 0);
	uint8_t endTransmission();
	uint8_t endTransmission(uint8_t sendStop);
	uint8_t requestFrom(int address, uint8_t dataLen, uint32_t command, uint8_t CMDLen, uint8_t sendStop, uint8_t is10BitAddr = 0);
	uint8_t pullFromSlaveRx();
	uint8_t write(uint8_t data);
	uint8_t write(uint8_t* data, int length);
	uint8_t peek();
	uint8_t read();
	uint8_t available();

	void set_RecieveCallback(void (*func)(uint8_t)) {
		user_onReceive = func;
	}
	void set_RequestCallback(void (*func)(void)) {
		user_onRequest = func;
	}
private:
	struct BufTable {
		int8_t Length;
		int8_t Index;
	};
	__aligned_four__ uint8_t TxBuf[32];
	__aligned_four__ uint8_t RxBuf[32];
	BufTable TX;
	BufTable RX;

	uint8_t transmitting;
	int targetAddr;
	uint8_t targetIs10bit;

	void (*user_onReceive)(uint8_t len);
	void (*user_onRequest)(void);
	void onDataArrived();
	void onDataRequest();

	static void txThunk(void* ctx)
	{
		static_cast<TwoWire*>(ctx)
			->onDataRequest();
	}

	static void rxThunk(void* ctx)
	{
		static_cast<TwoWire*>(ctx)
			->onDataArrived();
	}
};

#endif // !TwoWire_h
