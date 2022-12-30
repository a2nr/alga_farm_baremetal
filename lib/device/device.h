#ifndef DEVICE_H
#define DEVICE_H

#ifndef DEVICE_DEBUG
// device 1
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

// device 2
#define PIN_OUTPUT11     2
#define PIN_OUTPUT12     3
#define PIN_OUTPUT13     4
#define PIN_OUTPUT14     5
#define PIN_OUTPUT15     6
#define PIN_OUTPUT16     7
#define PIN_OUTPUT17     8
#define PIN_OUTPUT18     9
#define PIN_OUTPUT19    10
#define PIN_OUTPUT20    11
#define PIN_OUTPUT21    12

#define PIN_INPUT8      14
#define PIN_INPUT9      15
#define PIN_INPUT10     16
#define PIN_INPUT11     17
#define PIN_INPUT12     18
#define PIN_INPUT13     19
#define PIN_INPUT14     20
#define PIN_INPUT15     21
#else

// device 1
#define PIN_OUTPUT0      1
#define PIN_OUTPUT1      2
#define PIN_OUTPUT2      3
#define PIN_OUTPUT3      4
#define PIN_OUTPUT4      5
#define PIN_OUTPUT5      6
#define PIN_OUTPUT6      7
#define PIN_OUTPUT7      8

#define PIN_OUTPUT8      9
#define PIN_OUTPUT9     10
#define PIN_OUTPUT10    11

#define PIN_INPUT0      23
#define PIN_INPUT1      24
#define PIN_INPUT2      25
#define PIN_INPUT3      26
#define PIN_INPUT4      27
#define PIN_INPUT5      28
#define PIN_INPUT6      29
#define PIN_INPUT7      30

// device 2
#define PIN_OUTPUT11    12
#define PIN_OUTPUT12    13
#define PIN_OUTPUT13    14
#define PIN_OUTPUT14    15
#define PIN_OUTPUT15    16
#define PIN_OUTPUT16    17
#define PIN_OUTPUT17    18
#define PIN_OUTPUT18    19

#define PIN_OUTPUT19    20
#define PIN_OUTPUT20    21
#define PIN_OUTPUT21    22

#define PIN_INPUT8      31
#define PIN_INPUT9      32
#define PIN_INPUT10     33
#define PIN_INPUT11     34
#define PIN_INPUT12     35
#define PIN_INPUT13     36
#define PIN_INPUT14     37
#define PIN_INPUT15     38

#endif


#define PIN_LOW         0
#define PIN_HIGH        1
#define PIN_INPUT       0
#define PIN_OUTPUT      1


void init_device();
void start_device();
void initPin(unsigned char, unsigned char);
void setPin(unsigned char, unsigned char);
bool getPin(unsigned char);
#endif
