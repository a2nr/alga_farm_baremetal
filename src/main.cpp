#include <Arduino.h>
#include "device.h"

#if defined(DEVICE_DEBUG)


#include <ModbusRTUSlave.h>
#define MODBUS_ID			66
#define MODBUS_SIZE_BUFF 	256
#define MODBUS_BAUDRATE		38400
uint8_t modbusBuff[MODBUS_SIZE_BUFF];
ModbusRTUSlave modbus(Serial,modbusBuff,MODBUS_SIZE_BUFF);

#define IO_CHANNEL		2
unsigned char inputCh[IO_CHANNEL];
unsigned char outputCh[IO_CHANNEL];

#else


#ifdef ARDUINO_ARCH_AVR
#include <FrequencyTimer2.h>
#endif


#endif


void setup() {
#if defined(DEVICE_DEBUG)
	Serial.begin(MODBUS_BAUDRATE);
	modbus.begin(MODBUS_ID,MODBUS_BAUDRATE);
#endif
	init_device();

}



void loop() {
#if defined(DEVICE_DEBUG)
#endif
	start_device();
}



void freqGenerator_setPeriod(unsigned long period) 
{
#if defined(DEVICE_DEBUG)
#else
#ifdef ARDUINO_ARCH_AVR
	FrequencyTimer2::setPeriod(period);
#endif
#endif
}



void freqGenerator_setOut(bool enable)
{
#if defined(DEVICE_DEBUG)
#else
#ifdef ARDUINO_ARCH_AVR
	if(enable)
		FrequencyTimer2::enable();
	else
		FrequencyTimer2::disable();
#endif
#endif
}



void initPin(unsigned char pin, unsigned char mode)
{
#if defined(DEVICE_DEBUG)
#else
	pinMode(pin,mode);
#endif
}



void setPin(unsigned char pin, unsigned char value)
{
#if defined(DEVICE_DEBUG)
#else
	digitalWrite(pin,value);
#endif
}



bool getPin(unsigned char pin)
{
#if defined(DEVICE_DEBUG)
#else
#ifdef ARDUINO_ARCH_AVR
	if((pin == A6) || (pin == A7))
		return analogRead(pin) > 40;
#endif
	return digitalRead(pin);
#endif
}

unsigned long plclib_millis()
{
	return millis();
}