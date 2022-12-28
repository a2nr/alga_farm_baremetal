#include <stdio.h>
#include <modbus/modbus.h>
#include <device.h>
#include <freqGenerator.h>

int main(int argc, char *argv[])
{
    modbus_t *mb;

    mb = modbus_new_tcp("127.0.0.1",1502);
    modbus_connect(mb);

    printf("HELLOO COK, WE READY TO MODCOK!! \r");

    modbus_close(mb);
    modbus_free(mb);
}

void initPin(unsigned char, unsigned char)
{
}
void setPin(unsigned char, unsigned char)
{
}
bool getPin(unsigned char)
{
    return false;
}
void freqGenerator_setPeriod(unsigned char, unsigned long)
{
}
void freqGenerator_setOut(unsigned char, bool)
{

}
unsigned long plclib_millis()
{
        return 0;
}
