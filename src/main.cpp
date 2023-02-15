#include "device.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)
#if defined(DEVICE_DEBUG)
#define MODBUS_LAYER
#elif defined(BETA_TEST)
#else
#define FREQ_IMP
#endif
#endif

#ifdef FREQ_IMP
#include <FrequencyTimer2.h>
#endif

#if defined(MODBUS_LAYER)
#include <LibPrintf.h>
#include <SoftwareSerial.h>
#include <ModbusRTUSlave.h>
#define MB_BUFF_SIZE 	256
#define MB_NCOIL		56
#define MB_NREG			2
uint8_t buff[MB_BUFF_SIZE];
ModbusRTUSlave mb(Serial,buff,MB_BUFF_SIZE);

int8_t mbCoil[MB_NCOIL];
uint16_t mbReg[MB_NREG];

int8_t readCoil(uint16_t address)
{
    if(address > MB_NCOIL)
        return -1;
    return mbCoil[address];
}
boolean writeCoil(uint16_t address, boolean value)
{
    if(address > MB_NCOIL)
        return false;
    mbCoil[address] = value ? 1 : 0;
    return true;
}
int32_t readReg(uint16_t address)
{
    if(address > MB_NREG)
        return -1;
    return mbReg[address];
}
#endif

void setup() {
#ifdef MODBUS_LAYER
   Serial.begin(115200);
   mb.begin(1,115200);
   mb.configureCoils(MB_NCOIL,readCoil,writeCoil);
   mb.configureInputRegisters(MB_NREG,readReg);
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


void initPin(unsigned char pin, unsigned char mode)
{
#ifndef MODBUS_LAYER
	pinMode(pin,mode);
#endif
}


void setPin(unsigned char pin, unsigned char value)
{
#ifdef MODBUS_LAYER
    mbCoil[pin - 1] = value == PIN_HIGH ? 0 : 1;
#else
    digitalWrite(pin,value);
#endif
}


bool getPin(unsigned char pin)
{
#ifdef MODBUS_LAYER
    return mbCoil[pin-1] != 1 ? false : true;
#else
#if defined(FREQ_IMP) || defined(BETA_TEST)
    if((pin == A6) || (pin == A7))
        return analogRead(pin) > 40;
#endif
    return digitalRead(pin);
#endif
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
#if defined(MODBUS_LAYER) || defined(BETA_TEST)
    setPin(channel, !enable);
#endif
#ifdef FREQ_IMP
    if(enable)
        FrequencyTimer2::enable();
    else
        FrequencyTimer2::disable();    
#endif
}

unsigned long plclib_millis()
{
    return millis();
}
