/* Copyright (c) 2011, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/


#include "CDC.h"
#include "ISP583.h"
#include "CH583SFR.h"

#define printLog 0


/*9*/ConfigDescriptor USB_VFG = D_CONFIG(80, 4);

__IAP_FLAG unsigned short iap_magic_num;


volatile LineInfo _usbLineInfo = { 9600, 0x00, 0x00, 0x08, 0x00 };
uint8_t serial_state_notify[10] = {
	0xA1,       // bmRequestType
	0x20,       // bNotification = SERIAL_STATE
	0x00, 0x00, // wValue
	0x00, 0x00, // wIndex (interface 0)
	0x02, 0x00, // wLength = 2
	0x03, 0x00  // UART State: DCD | DSR
};
static volatile int32_t breakValue = -1;

CDC_& CDC()
{
	static CDC_ obj;
	return obj;
}

int CDC_::getInterface(uint8_t* interfaceCount, int offset) {
	CDCDescriptor _cdcInterface = {
	D_IAD(*interfaceCount, 2, CDC_COMMUNICATION_INTERFACE_CLASS, CDC_ABSTRACT_CONTROL_MODEL, 0x00),

	D_INTERFACE(*interfaceCount, 1, 0x02, 0x02, 0x00),	//face 0, 1 EP, CDC COMM, ACM, AT
	D_CDCCS(CDC_HEADER, 0x10, 0x01),	//header, v1.10
	D_CDCCS(CDC_CALL_MANAGEMENT, 0x01, (uint8_t)(*interfaceCount + 1)),	//Call Management, No data IF inside, Data InterFace
	D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT, 0x06),	//ACM, linecoding linestate
	D_CDCCS(CDC_UNION, *interfaceCount, (uint8_t)(*interfaceCount + 1)),	//CDC_UNION, COMM interface, DATA interface
	D_ENDPOINT(0x81, 0x03, 0x08, 0x10), //EP1 IN, INT, 8 bytes, interval 16ms

	D_INTERFACE((uint8_t)(*interfaceCount + 1), 2, CDC_DATA_INTERFACE_CLASS, 0x00, 0x00), //face 1, 2 EP, CDC DATA, NONE, NONE
	D_ENDPOINT(0x02, 0x02, 0x40, 0x00),	//EP2 OUT, BULK, 64 bytes, ignor
	D_ENDPOINT(0x83, 0x02, 0x40, 0x00),	//EP3 IN, BULK, 64 bytes, ignor
	};
	*interfaceCount += 2;

	return USB_FillBuffer(offset, &_cdcInterface, sizeof(_cdcInterface));
}

uint8_t CDC_::getShortName(char* name) {
	name[0] = 'C';
	name[1] = 'H';
	name[2] = '5';
	name[3] = '8';
	name[4] = 'x';
	name[5] = 'C';
	name[6] = 'D';
	name[7] = 'C';
	return 8;
}

bool need_serial_state_notify = 0;



bool CDC_::setup(USBSetup& setup) {
	uint8_t r = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;
	uint8_t* EP0_Buffer = USBDevice.getEpBuffer(0);
	
	if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType)
	{
		if (CDC_GET_LINE_CODING == r)
		{
			memcpy(EP0_Buffer, (const void*)&_usbLineInfo, 7);			
#if(printLog)
			Serial1.print("GET_LINE_CODING : send ");
			Serial1.println(_usbLineInfo.dwDTERate);
#endif
			x8_UEP0_T_LEN.length = 7;
			//PFIC_DisableIRQ(USB_IRQn);
			R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
			//PFIC_EnableIRQ(USB_IRQn);			
			return true;
		}
		else {
#if(printLog)
			Serial1.println("I'm groot");
#endif
		}
	}
	else if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType)
	{
		if (CDC_SEND_BREAK == r)
		{
#if(printLog)
			Serial1.println("Brake Brake Brake");
#endif
			breakValue = ((uint16_t)setup.wValueH << 8) | setup.wValueL;
			x8_UEP0_T_LEN.length = 0;
			//PFIC_DisableIRQ(USB_IRQn);
			R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
			//PFIC_EnableIRQ(USB_IRQn);
		}
		else if (CDC_SET_LINE_CODING == r)
		{
			//PFIC_DisableIRQ(USB_IRQn);
			R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
			//PFIC_EnableIRQ(USB_IRQn);
		}
		else if (CDC_SET_CONTROL_LINE_STATE == r)
		{
			
			_usbLineInfo.lineState = setup.wValueL;
#if(printLog)
			Serial1.print("SCLS: ");
			Serial1.println(_usbLineInfo.lineState, HEX);
			Serial1.println((uintptr_t)&iap_magic_num, HEX);
#endif			
			x8_UEP0_T_LEN.length = 0;
			//PFIC_DisableIRQ(USB_IRQn);
			R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
			//PFIC_EnableIRQ(USB_IRQn);
			
			// We check DTR state to determine if host port is open (bit 0 of lineState).
			if (1200 == _usbLineInfo.dwDTERate && (_usbLineInfo.lineState & 0x01) == 0)
			{
				iap_magic_num = 0x55AA;
				PFIC_SystemReset();
			}
		}
		else {
#if(printLog)
			Serial1.println("I'm happy");
#endif
			return false;
		}
		return true;
	}
	else {
#if(printLog)
		Serial1.println(requestType, HEX);
#endif
	}
	return false;
}

int CDC_::getDescriptor(USBSetup& setup) {
#if(printLog)
	Serial1.println("Go Hell!");
#endif
	return 0;
}


void Serial_::begin(unsigned long /* baud_count */)
{
	peek_buffer = -1;
}

void Serial_::begin(unsigned long /* baud_count */, uint8_t /* config */)
{
	peek_buffer = -1;
}

void Serial_::end(void)
{
}

int Serial_::available(void)
{
	if (peek_buffer >= 0) {
		return 1 + USB_Available(CDC_RX);
	}
	return USB_Available(CDC_RX);
}

int Serial_::peek(void)
{
	if (peek_buffer < 0)
		peek_buffer = USB_Recv(CDC_RX);
	return peek_buffer;
}

int Serial_::read(void)
{
	if (peek_buffer >= 0) {
		int c = peek_buffer;
		peek_buffer = -1;
		return c;
	}
	return USB_Recv(CDC_RX);
}

int Serial_::availableForWrite(void)
{
	return 64;
}

void Serial_::flush(void)
{

}

size_t Serial_::write(uint8_t c)
{
	return write(&c, 1);
}

size_t Serial_::write(const uint8_t* buffer, size_t size)
{
	if (dtr() > 0) {
		int r = USB_Send(CDC_TX, buffer, size);
		if (r > 0) {
			return r;
		}
		else {
			setWriteError();
			return 0;
		}
	}
	setWriteError();
	return 0;
}

CDC_::CDC_() :PluggableUSBModule(3, 2, epType) {
	Type = USBModuleType::USB_CDC;
	epType[0] = EP_TYPE_INTERRUPT_IN;		// EP1 IN  (通知端點)
	epType[1] = EP_TYPE_BULK_OUT;			// EP2 OUT (資料接收)
	epType[2] = EP_TYPE_BULK_IN;			// EP3 IN  (資料傳送)
	PluggableUSB().plug(this);
}

Serial_::Serial_() { 
	CDC();
	peek_buffer = -1; 
};
Serial_::operator bool() {
	bool result = false;
	if (_usbLineInfo.lineState > 0)
		result = true;
	delay(10);
	return result;
}

unsigned long Serial_::baud() {
	uint32_t baudrate;
	baudrate = _usbLineInfo.dwDTERate;
	return baudrate;
}

uint8_t Serial_::stopbits() {
	return _usbLineInfo.bCharFormat;
}

uint8_t Serial_::paritytype() {
	return _usbLineInfo.bParityType;
}

uint8_t Serial_::numbits() {
	return _usbLineInfo.bDataBits;
}

bool Serial_::dtr() {
	return _usbLineInfo.lineState & 0x1;
}

bool Serial_::rts() {
	return _usbLineInfo.lineState & 0x2;
}

int32_t Serial_::readBreak() {
	int32_t ret;
	ret = breakValue;
	breakValue = -1;
	return ret;
}

Serial_ Serial;