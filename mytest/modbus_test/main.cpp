#include <cstdint>
#include <cstdlib>
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

#ifndef WORKS_IP
#define  WORKS_IP       "0.0.0.0"
#endif
#ifndef WORKS_PORT
#define  WORKS_PORT     1502
#endif

#define TABS_BITNB  50

volatile bool runIt;

uint8_t *tab_bits; //[TABS_BITNB];

void stopIt(int);

int makeNewConnection(int masterSocket, fd_set *refSDesc, int maxFDesc);
int handleIncomingData(modbus_t * Context, modbus_mapping_t * registerMap, int curenMSocket, fd_set * refSDesc, int maxFDesc);

int main(void)
{

  modbus_mapping_t *registerMap;
  modbus_t *Context;
  int masterSocket = -1;
  fd_set refSDesc;
  fd_set setSDesc;
  int maxFDesc;
  int curenMSocket;
  struct timeval timeout;
  timeout.tv_sec = 3;

  signal(SIGINT, stopIt);

  runIt = true;

  Context = modbus_new_tcp(WORKS_IP,WORKS_PORT);

  if(Context == NULL)
    {
      printf("[SYS]\t\t Failed to initialize modbus tcp %s\r", modbus_strerror(errno));
      modbus_free(Context);
      return -1;
    }

  modbus_set_debug(Context, TRUE);

  registerMap = modbus_mapping_new(
        TABS_BITNB /*bit*/,
        0 /*in bit*/, 0 /*h register*/, 0 /*in register*/);


  if(registerMap == NULL )
    {
      printf("[SYS]\t\t Failed to init register map %s\r\n", modbus_strerror(errno));

      modbus_mapping_free(registerMap);
      modbus_free(Context);
      return -1;
    }
//  registerMap->tab_bits = (tab_bits = (uint8_t*)malloc(TABS_BITNB * sizeof(uint8_t)));
  tab_bits = registerMap->tab_bits;

  try
  {
    init_device();
    start_device();
  }
  catch (const char * e)
  {
    printf("[SYS]\t\t Failed to init device \r\n");
    printf("[DVS]\t\t\t\t %s", e);

    modbus_mapping_free(registerMap);
    modbus_free(Context);
    return -1;
  }



  registerMap->tab_bits = tab_bits;

  printf("[SYS]\t\t HELLOO COK, WE READY TO MODCOK!! \r\n");
  printf("[SYS]\t\t IP : %s/%d \r\n", WORKS_IP, WORKS_PORT);

  masterSocket = modbus_tcp_listen(Context, 1);

  FD_ZERO(&refSDesc);
  FD_SET(masterSocket,&refSDesc);

  maxFDesc = masterSocket;


  while(runIt)
    {
      usleep(500);
      setSDesc = refSDesc;
      if(select(maxFDesc+1, &setSDesc, NULL, NULL, &timeout) == -1)
        {
          printf("[SYS]\t\t select() failure.\r\n");
          continue;
        }

      for(curenMSocket = 0; curenMSocket <= maxFDesc; curenMSocket++)
        {
          if(!FD_ISSET(curenMSocket, &setSDesc))
            continue;

          if(curenMSocket != masterSocket)
            {
              maxFDesc = handleIncomingData(Context, registerMap, curenMSocket, &refSDesc, maxFDesc);
              continue;
            }
          int newFDesc = makeNewConnection(masterSocket, &refSDesc, maxFDesc);

          maxFDesc = newFDesc != -1 ? newFDesc : maxFDesc;
        }
    }

  printf("[SYS]\t\t Quit the loop: %s \r\n", modbus_strerror(errno));

  if(masterSocket != -1)
    {
      close(masterSocket);
    }

  modbus_mapping_free(registerMap);

  modbus_close(Context);
  modbus_free(Context);
}

void stopIt(int)
{
  runIt = false;
  printf("someone press CTRL+C, program gonna stop\r\n");
}

int handleIncomingData(modbus_t * Context, modbus_mapping_t * registerMap, int curenMSocket, fd_set * refSDesc, int maxFDesc)
{
  uint8_t aduBuffer[MODBUS_TCP_MAX_ADU_LENGTH];
  int data = 0;
  int address = 0;
  int rc;

  modbus_set_socket(Context, curenMSocket);
  rc = modbus_receive(Context, aduBuffer);

  if(rc != -1)
    {
      modbus_reply(Context, aduBuffer, rc, registerMap);
      try
      {
        start_device();
      }
      catch(const char * e)
      {
        printf("[SYS]\t\t Throw Exception on start device \r\n");
        printf("[DVS]\t\t\t\t %s\r\n", e);
      }
      return maxFDesc;
    }
  printf("[SYS]\t\t Connection closed on socket %d\r\n", curenMSocket);
  close(curenMSocket);

  FD_CLR(curenMSocket, refSDesc);

  if(curenMSocket == maxFDesc)
    maxFDesc--;
  return maxFDesc;
}

int makeNewConnection(int masterSocket, fd_set *refSDesc, int maxFDesc)
{
  socklen_t addrlen;
  struct sockaddr_in clientaddr;
  int newSDesc;

  addrlen = sizeof(clientaddr);
  memset(&clientaddr, 0, sizeof(clientaddr));
  newSDesc = accept(masterSocket,(struct sockaddr *)&clientaddr, &addrlen);

  if(newSDesc == -1)
    {
      printf("[SYS]\t\t accept() error\r\n");
      return newSDesc;
    }

  FD_SET(newSDesc, refSDesc);

  printf("[SYS]\t\t new Connection establised!\r\n" );

  if(newSDesc > maxFDesc)
    return newSDesc;

  return maxFDesc;
}

void initPin(unsigned char pin, unsigned char role)
{
  if(tab_bits == NULL)
    throw "tab bits not allocated";
  if(pin >TABS_BITNB)
    throw "need more bits";
}
void setPin(unsigned char pin, unsigned char logic)
{
  initPin(pin, PIN_OUTPUT);
  tab_bits[pin - 1] = logic == PIN_HIGH ? FALSE : TRUE;
  printf("[DVS]\t\t setPin() on %d as %s \r\n", pin, tab_bits[pin - 1] ? "FALSE" : "TRUE");
}

bool getPin(unsigned char pin)
{
  bool logic;

  initPin(pin, PIN_OUTPUT);
  logic = tab_bits[pin - 1] != TRUE ? false : true;
  printf("[DVS]\t\t getPin() on %d -> %s\r\n", pin, logic ? "FALSE" : "TRUE");
  return logic;
}
void freqGenerator_setPeriod(unsigned char pin, unsigned long period)
{
  printf("[DVS]\t\t freqGenerator_setPeriod() %c  %lu\r\n", pin, period);
}
void freqGenerator_setOut(unsigned char pin, bool logic)
{
  printf("[DVS]\t\t freqGenerator_setOut() %c  %d\r\n", pin, logic);
  setPin(pin,logic);
}
unsigned long plclib_millis()
{
  unsigned long millis = clock();
  printf("[DVS]\t\t plclib_milis() -> %lu\r\n", millis);
  return millis;
}
