#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <modbus/modbus.h>

#include <device.h>
#include <freqGenerator.h>

#define  WORKS_IP       "0.0.0.0"
#define  WORKS_PORT     1502

volatile bool runIt = true;

modbus_mapping_t *registerMap;

uint16_t bitCoilMaxAlocation;
uint16_t bitIntpMaxAlocation;


void stopIt(int)
{
        runIt = false;
        printf("someone press CTRL+c, program stop");
}

int main(void)
{
        modbus_t *Context;
        int masterSocket = -1;
        int rc;
        fd_set refSDesc;
        fd_set setSDesc;
        int maxFDesc;
        int curenMSocket;
        struct timeval timeout;
        timeout.tv_sec = 3;

        signal(SIGINT, stopIt);

        Context = modbus_new_tcp(WORKS_IP,WORKS_PORT);

        bitCoilMaxAlocation = 0;
        bitIntpMaxAlocation = 0;

        init_device();

        registerMap = modbus_mapping_new(
                    bitCoilMaxAlocation /*bit*/,
                    bitIntpMaxAlocation /*in bit*/,
                    0 /*h register*/, 0 /*in register*/);

        registerMap->tab_bits = (uint8_t*) malloc(sizeof(uint8_t)*bitCoilMaxAlocation);


        modbus_set_debug(Context, TRUE);

        masterSocket = modbus_tcp_listen(Context, 1);

        if(registerMap == NULL || Context == NULL)
        {
                printf("[SYS]\t\t Failed to init %s\r", modbus_strerror(errno));

                modbus_free(Context);
                return -1;
        }

        printf("[SYS]\t\t HELLOO COK, WE READY TO MODCOK!! \r\n");
        printf("[SYS]\t\t IP : %s/%d \r\n", WORKS_IP, WORKS_PORT);

        FD_ZERO(&refSDesc);
        FD_SET(masterSocket,&refSDesc);

        maxFDesc = masterSocket;


        while(runIt)
        {
                usleep(500);
                setSDesc = refSDesc;
                if(select(maxFDesc+1, &setSDesc, NULL, NULL, &timeout) == -1)
                {
                        printf("[SYS]\t\t select() failure.");
                        continue;
                }

                for(curenMSocket = 0; curenMSocket <= maxFDesc; curenMSocket++)
                {
                        if(!FD_ISSET(curenMSocket, &setSDesc))
                                continue;

                        if(curenMSocket != masterSocket)
                        {
                                uint8_t aduBuffer[MODBUS_TCP_MAX_ADU_LENGTH];
                                int data = 0;
                                int address = 0;


                                modbus_set_socket(Context, curenMSocket);
                                rc = modbus_receive(Context, aduBuffer);

                                if(rc != -1)
                                {
                                        modbus_reply(Context, aduBuffer, rc, registerMap);
                                        start_device();

                                        continue;
                                }
                                printf("[SYS]\t\t Connection closed on socket %d\n", curenMSocket);
                                close(curenMSocket);

                                FD_CLR(curenMSocket, &refSDesc);

                                if(curenMSocket == maxFDesc)
                                        maxFDesc--;

                                continue;

                        }

                        socklen_t addrlen;
                        struct sockaddr_in clientaddr;
                        int newSDesc;

                        addrlen = sizeof(clientaddr);
                        memset(&clientaddr, 0, sizeof(clientaddr));
                        newSDesc = accept(masterSocket,(struct sockaddr *)&clientaddr, &addrlen);

                        if(newSDesc == -1)
                        {
                                printf("[SYS]\t\t accept() error");
                                continue;
                        }

                        FD_SET(newSDesc, &refSDesc);

                        if(newSDesc > maxFDesc)
                                maxFDesc = newSDesc;

                        printf("[SYS]\t\t new Connection ok!\r\n" );

                }
        }

        printf("[SYS]\t\t Quit the loop: %s \r", modbus_strerror(errno));

        if(masterSocket != -1)
        {
                close(masterSocket);
        }

        modbus_mapping_free(registerMap);

        modbus_close(Context);
        modbus_free(Context);
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
        return clock();
}
