#include "device.h"
#include <Arduino.h>
#include <FrequencyTimer2.h>

void setup() {
    init_device();
}



void loop() {
    start_device();
    delay(50);
}



void freqGenerator_setPeriod(unsigned char channel, unsigned long period) 
{
#ifdef ARDUINO_ARCH_AVR
	FrequencyTimer2::setPeriod(period);
#endif
}



void freqGenerator_setOut(unsigned char channel, bool enable)
{
#ifdef ARDUINO_ARCH_AVR
	if(enable)
		FrequencyTimer2::enable();
	else
		FrequencyTimer2::disable();
#endif
}



void initPin(unsigned char pin, unsigned char mode)
{
	pinMode(pin,mode);
}



void setPin(unsigned char pin, unsigned char value)
{
	digitalWrite(pin,value);
}


bool getPin(unsigned char pin)
{
#ifdef ARDUINO_ARCH_AVR
	if((pin == A6) || (pin == A7))
		return analogRead(pin) > 40;
#endif
	return digitalRead(pin);
}

unsigned long plclib_millis()
{
    return millis();
}
