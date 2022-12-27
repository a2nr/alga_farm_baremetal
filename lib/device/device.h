#ifndef DEVICE_H
#define DEVICE_H

#define PIN_OUTPUT0      2
#define PIN_OUTPUT1      3
#define PIN_OUTPUT2      4
#define PIN_OUTPUT3      5
#define PIN_OUTPUT4      6
#define PIN_OUTPUT5      7
#define PIN_OUTPUT6      8
#define PIN_OUTPUT7      9

#define PIN_OUTPUT8     10
#define PIN_OUTPUT9     11
#define PIN_OUTPUT10    12

#define PIN_INPUT0      14
#define PIN_INPUT1      15
#define PIN_INPUT2      16
#define PIN_INPUT3      17
#define PIN_INPUT4      18
#define PIN_INPUT5      19
#define PIN_INPUT6      20
#define PIN_INPUT7      21


#define     PIN_LOW     0
#define     PIN_HIGH    1
#define     PIN_INPUT   0
#define     PIN_OUTPUT  1


void init_device();
void start_device();
void initPin(unsigned char, unsigned char);
void setPin(unsigned char, unsigned char);
bool getPin(unsigned char);
#endif
