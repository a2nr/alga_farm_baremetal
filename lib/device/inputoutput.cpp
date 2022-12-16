#include "device.h"

#if defined(DEVICE_1) || defined(DEVICE_2)

#include <Arduino.h>
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
	return digitalRead(pin);
}
#else
void initPin(unsigned char pin, unsigned char mode)
{
}
void setPin(unsigned char pin, unsigned char value)
{
}
bool getPin(unsigned char pin)
{
}
#endif


