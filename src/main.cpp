#include "device.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)
#if defined(DEVICE_DEBUG)
#define MODBUS_LAYER
#else
#define FREQ_IMP
#endif
#endif

#ifdef FREQ_IMP
#include <FrequencyTimer2.h>
#endif

#if defined(MODBUS_LAYER)
#include <ModbusRTUSlave.h>
#define MB_BUFF_SIZE 	256
#define MB_NCOIL		56
#define MB_NREG			2
uint8_t buff[MB_BUFF_SIZE];
ModbusRTUSlave mb(Serial,buff,MB_BUFF_SIZE);

uint8_t statusPin[MB_NCOIL];
int8_t mbCoil[MB_NCOIL];
uint16_t mbReg[MB_NREG];

int8_t readCoil(uint16_t address)
{
    if(address > MB_NCOIL)
        return -1;
    return mbCoil[address-1];
}
boolean writeCoil(uint16_t address, boolean value)
{
    if(address > MB_NCOIL)
        return -1;

    if(statusPin[address-1]==PIN_OUTPUT)
    {
        mbCoil[address-1] = !value;
        return true;
    }

    mbCoil[address-1] = value;
    return true;
}
int32_t readReg(uint16_t address)
{
    if(address > MB_NREG)
        return -1;
    return mbReg[address-1];
}
boolean writeReg(uint16_t address, uint16_t value)
{
    if(address > MB_NREG)
        return false;
    mbReg[address-1] = value;
    return true;
}
#endif

void setup() {
#ifdef MODBUS_LAYER
   Serial.begin(115200);
   mb.begin(1,115200);
   mb.configureCoils(MB_NCOIL,readCoil,writeCoil);
   mb.configureInputRegisters(MB_NREG,readReg);
   memset(statusPin,3,sizeof(uint8_t)*MB_NCOIL);
#endif
    init_device();
}



void loop() {
#ifdef MODBUS_LAYER
    mb.poll();
#else
    delay(50);
#endif
    start_device();
}



void freqGenerator_setPeriod(unsigned char channel, unsigned long period) 
{
#ifdef MODBUS_LAYER
    mbReg[channel] = period;
#endif
#ifdef FREQ_IMP
	FrequencyTimer2::setPeriod(period);
#endif
}



void freqGenerator_setOut(unsigned char channel, bool enable)
{
#ifdef MODBUS_LAYER
   mbCoil[channel-1] = enable;
#endif
#ifdef FREQ_IMP
	if(enable)
		FrequencyTimer2::enable();
	else
		FrequencyTimer2::disable();
#endif
}



void initPin(unsigned char pin, unsigned char mode)
{
#ifdef MODBUS_LAYER
    statusPin[pin-1] = mode;
#else
	pinMode(pin,mode);
#endif
}



void setPin(unsigned char pin, unsigned char value)
{
#ifdef MODBUS_LAYER
    if((statusPin[pin-1] != PIN_OUTPUT) && (statusPin[pin-1] != PIN_INPUT))
    {
        statusPin[pin-1] = PIN_OUTPUT;
    }
    if(statusPin[pin-1] == PIN_OUTPUT)
    {
        mbCoil[pin-1] = value > 0 ? 0 : 1;
        return;
    }
    mbCoil[pin-1] = value > 0 ? 1 : 0;
#else
    digitalWrite(pin,value);
#endif
}


bool getPin(unsigned char pin)
{
#ifdef MODBUS_LAYER
    if(statusPin[pin-1] == PIN_OUTPUT)
        return mbCoil[pin-1] > 0 ? false : true;
    return mbCoil[pin-1] > 0 ? true : false;
#else
#ifdef FREQ_IMP
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
